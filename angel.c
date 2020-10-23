#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "funlist.h"
#include "apmath.h"

/*
 ****************************************************************
 ****************************************************************
 */

struct angel_header {
	struct angel_header *prev;
	struct angel_header *next;
	int flag;
};

int angel_header_prev(
	void *param,
	void *lst,
	void ***p_link)
{
	struct angel_header *head = (struct angel_header *)lst;

	if(lst == NULL || p_link == NULL) {
		return 1;
	}

	*p_link = (void **)&head->prev;

	return 0;
}
int angel_header_next(
	void *param,
	void *lst,
	void ***p_link)
{
	struct angel_header *head = (struct angel_header *)lst;

	if(lst == NULL || p_link == NULL) {
		return 1;
	}

	*p_link = (void **)&head->next;

	return 0;
}

int angel_header_size(struct angel_header *head)
{
	int size;

	size = (char *)head->next - (char *)&head[1];

	return size;
}

/*
 ****************************************************************
 ****************************************************************
 */

struct free {
	struct angel_header *backward;
	struct angel_header *forward;
};
struct lock {
	struct funlist_cyclic_pointer *angel_ring;
	struct funlist_cyclic_pointer *link;
};
#define FREE(head)	((struct free *)&((struct angel_header *)head)[1])
#define LOCK(head)	((struct lock *)&((struct angel_header *)head)[1])
#define LOCK_DATA(head)	(void *)(&LOCK(head)[1])
#define LOCK_LINK(head)	(LOCK(head)->link)

int angel_free_header_prev(
	void *param,
	void *lst,
	void ***p_link)
{
	if(lst == NULL || p_link == NULL) {
		return 1;
	}

	*p_link = (void **)&FREE(lst)->backward;

	return 0;
}
int angel_free_header_next(
	void *param,
	void *lst,
	void ***p_link)
{
	if(lst == NULL || p_link == NULL) {
		return 1;
	}

	*p_link = (void **)&FREE(lst)->forward;

	return 0;
}

int is_lock(struct angel_header *head)
{
	if((char *)LOCK(head)->link > (char *)head
		&& (char *)LOCK(head)->link < (char *)head->next)
	{
		return 1;
	}
	return 0;
}

int angel_lock_header_link(struct angel_header *head)
{
	int link;

	link = (struct funlist_cyclic_pointer *)head->next - LOCK_LINK(head);

	return link;
}
int angel_lock_header_size(struct angel_header *head)
{
	int size;

	size = (char *)LOCK(head)->link
		- (char *)LOCK_DATA(head);

	return size;
}

int print(struct angel_header *head)
{
	struct angel_header *temp;

	temp = head;
	while(temp != NULL) {
		int i;
		int use;

		use = is_lock(temp);
		if(use) {
			printf("LOCK");
		} else {
			printf("FREE");
		}
		printf("(%x): prev(%x), next(%x), flag(%x)\n",
			temp, temp->prev, temp->next, temp->flag);

		if(use) {
			int size;
			int link;

			size = angel_lock_header_size(temp);
			link = angel_lock_header_link(temp);
			printf("\tsize, link, angel = %d, %d, %x\n",
				size, link, LOCK(temp)->angel_ring);
			for(i = 0; i < link; ++i) {
				printf("\tlink[%d](%x): prev(%x), next(%x), memory(%x)\n",
					i, &LOCK(temp)->link[i],
					LOCK(temp)->link[i].prev,
					LOCK(temp)->link[i].next,
					LOCK(temp)->link[i].pointer);
			}
		} else {
			int size;

			size = angel_header_size(temp);
			printf("\tsize = %d\n", size);
			printf("\tbackward(%x), forward(%x)\n",
				FREE(temp)->backward,
				FREE(temp)->forward);
		}
		temp = temp->next;
	}
}

/**
 ****************************************************************
 */

int angel_free_header_init(
	struct angel_header *head,
	int size)
{
	//head->prev = NULL;
	funlist_list_init(
		NULL,
		angel_header_prev,
		head,
		NULL);
	//head->next = (struct angel_header *)((char *)head + size);
	funlist_list_init(
		NULL,
		angel_header_next,
		head,
		(char *)head + size);

	funlist_cyclic_list_init(
		NULL, angel_free_header_prev,
		NULL, angel_free_header_next,
		head);

	return 0;
}

int angel_free_header_trim(
	struct angel_header *head,
	int size)
{
	struct angel_header *tail;

	tail = (struct angel_header *)
		((char *)&head[1] + size);
	tail->next = head->next;
	head->next = tail;

	funlist_cyclic_list_insert(
		NULL, angel_free_header_prev,
		NULL, angel_free_header_next,
		head, head);

	funlist_cyclic_list_init(
		NULL, angel_free_header_prev,
		NULL, angel_free_header_next,
		tail);

	return 0;
}

int angel_header_split(
	struct angel_header *head,
	int size,
	int length)
{
	int i;
	struct funlist_cyclic_pointer *link;
	struct angel_header *tail;

	funlist_cyclic_list_insert(
		NULL, angel_free_header_prev,
		NULL, angel_free_header_next,
		head, head);

	link = (struct funlist_cyclic_pointer *)((char *)LOCK_DATA(head) + size);

	angel_free_header_trim(head, sizeof(struct lock) + size +
		sizeof(struct funlist_cyclic_pointer) * length);

	LOCK(head)->link = link;
	LOCK(head)->angel_ring = NULL;

	for(i = 0; i < length; ++i) {
		funlist_cyclic_list_init(
			NULL, funlist_cyclic_pointer_prev,
			NULL, funlist_cyclic_pointer_next,
			&link[i]);
		link[i].pointer = NULL;
	}
}

/**
 ****************************************************************
 * @details
 *	The structure of angel ring is
 *		    ... ... ... ... ... ... ...
 *		    V                         ^
 *		  prev   -->   angel   -->   next
 *		(latest)      (oldest)   (2nd oldest)
 *		                 ^
 *		                 |
 *		               target
 ****************************************************************
 */
int angel_lock_header_set_link(
	struct angel_header *head,
	int n,
	struct angel_header *object)
{
	int is_single;
	struct angel_header *target;

/**
 * 1.	The link[n] is refreshed. The content link[n].memory are
 *	going to be deleted. Before the delete, confirm that the
 *	link[n] is not directly pointed from the pointing object
 *	(target), that is, link[n] is not saved in the field
 *	angel. But, if nothing was formerly pointed by link[n],
 *	do nothing. When the link[n] is direct angel of the
 *	pointing object, check whether link[n] is the only one
 *	pointer that is pointing the target. If so, there would
 *	be no pointer referencing target, then the angel of the
 *	target becomes empty. Otherwise, update the angel of the
 *	target to the NEXT of the angel ring pointer. Afte
 *	completing the update of angel, kick out link[n] from the
 *	angel ring.
 */
	target = LOCK(head)->link[n].pointer;
	if(target != NULL) {
		if(LOCK(target)->angel_ring == &LOCK(head)->link[n]) {
			funlist_list_is_last(NULL,
				funlist_cyclic_pointer_prev,
				&LOCK(head)->link[n],
				&LOCK(head)->link[n],
				&is_single);

			if(is_single) {
				LOCK(target)->angel_ring = NULL;
			} else {
				// Angel becomes next of link[n], which is
				// relatively old reference to the target.
				LOCK(target)->angel_ring =
					LOCK(head)->link[n].next;
			}
		}
	}
	funlist_cyclic_list_insert(
		NULL, funlist_cyclic_pointer_prev,
		NULL, funlist_cyclic_pointer_next,
		&LOCK(head)->link[n],
		&LOCK(head)->link[n]);

/**
 * 2.	The content of link[n] is settled. The substituded
 *	value is the input object. The input object is empty,
 *	or object is newly referred, the topology of pointer
 *	should remain initialized, so the pointer remains to
 *	be a self loop. If the input object is not empty and
 *	the object is already referred by other pointer, we
 *	have to add the link[n] to the existing angel ring
 *	pointers. Be careful that we locate link[n] before
 *	the angel, that is, link[n] is PREV of angel. This
 *	implies the link[n] is the lates reference to the
 *	input object. On the other hand, when the object is
 *	newly referred, the angel must also registered for
 *	the first time.
 */
	LOCK(head)->link[n].pointer = object;
	if(object != NULL) {
		if(LOCK(object)->angel_ring != NULL) {
			// link[n] is located just before angel, which is
			// the latest reference to the object.
			funlist_cyclic_list_insert(
				NULL, funlist_cyclic_pointer_prev,
				NULL, funlist_cyclic_pointer_next,
				&LOCK(head)->link[n],
				LOCK(object)->angel_ring);
			return 0;
		}
		LOCK(object)->angel_ring = &LOCK(head)->link[n];
	}
#if 0
	// No need because link[n] is initialized at the
	// last of phase 1, when link[n] is omitted.
	funlist_cyclic_list_init(
		NULL, funlist_cyclic_pointer_prev,
		NULL, funlist_cyclic_pointer_next,
		&LOCK(head)->link[n]);
#endif

	return 0;
}

int migrate(
	struct angel_header *from,
	struct angel_header *to)
{
	int i;
	int length;
	int size;
	struct funlist_cyclic_pointer *temp;

	length = angel_lock_header_link(from);
	size = angel_lock_header_size(from);
	angel_header_split(to, size, length);

	funlist_list_map(
		NULL,				// link_param
		funlist_cyclic_pointer_prev,	// link
		to,				// func_param
		funlist_cyclic_pointer_set,	// func
		NULL,				// cond_param
		funlist_default_condition,	// cond
		LOCK(from)->angel_ring,		// lst
		LOCK(from)->angel_ring,		// sen
		NULL);				// last

	LOCK(to)->angel_ring = LOCK(from)->angel_ring;
	LOCK(from)->angel_ring = NULL;

	for(i = 0; i < length; ++i) {
		angel_lock_header_set_link(to, i, LOCK(from)->link[i].pointer);
		angel_lock_header_set_link(from, i, NULL);
	}
}

struct angel_header buffer[1000];
struct angel_header *pool;

#if 1
int main()
{
	struct angel_header *arena, *proc;
	struct angel_header *head1, *head2, *head3, *head4;
	struct angel_header *variable;

	printf("hello, world!\n");
	printf("sizeof(pointer) = %d\n", sizeof(struct funlist_cyclic_pointer));
	printf("sizeof(angel_header) = %d\n", sizeof(struct angel_header));

	angel_free_header_init(buffer, sizeof(buffer));

	pool = buffer;
	printf("link, size = %d, %d\n",
		angel_lock_header_link(pool), angel_lock_header_size(pool));

	print(buffer);
	printf("################################\n"); fflush(stdout);

#if 0
	angel_header_split(pool, 1024, 8); proc = pool; pool = pool->next;
	angel_lock_header_set_link(proc, 0, proc);
#endif

	angel_header_split(pool, 0, 8);
	variable = pool;
	pool = pool->next;

	angel_header_split(pool, 8, 1);
	strcpy(LOCK_DATA(pool), "add");
	angel_lock_header_set_link(variable, 0, pool);
	pool = pool->next;

	angel_header_split(pool, 8, 1);
	strcpy(LOCK_DATA(pool), "square");
	angel_lock_header_set_link(variable, 1, pool);
	pool = pool->next;

	angel_header_split(pool, sizeof(int), 3);
	*(int *)LOCK_DATA(pool) = 0;
	pool = pool->next;

	angel_header_split(pool, sizeof(int), 3);
	*(int *)LOCK_DATA(pool) = 1;
	pool = pool->next;

//	angel_header_split(pool, 10, 1); head1 = pool; pool = pool->next;
//	angel_header_split(pool, 20, 2); head2 = pool; pool = pool->next;
//	angel_header_split(pool, 30, 3); head3 = pool; pool = pool->next;
//	angel_lock_header_set_link(head1, 0, head2);
//	angel_lock_header_set_link(head2, 0, head1);
//	angel_lock_header_set_link(head3, 0, head1);

	print(buffer);
	printf("################################\n"); fflush(stdout);

//	migrate(head2, pool); head4 = pool; pool = pool->next;
//	print(buffer);
}
#endif
