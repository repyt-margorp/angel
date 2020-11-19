/**
 ****************************************************************
 *	Angel System
 * @file
 *	angel.c
 * @author
 *	Ryota MURAI
 * @date
 *	2020/11/18
 * @details
 *	Angel System!!!!!
 ****************************************************************
 */

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

/**
 ****************************************************************
 * @brief
 *	Get-function for prev of angel_header.
 * @param[in] param
 *	Parameter, which is not used.
 * @param[out] p_link
 *	Prev pointer.
 * @param[in] lst
 *	Angel header.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_header_prev(
	void *param,
	void ***p_link,
	void *lst)
{
	struct angel_header *head =
		(struct angel_header *)lst;

	if(lst == NULL) {
		return 1;
	}

	if(p_link != NULL) {
		*p_link = (void **)&head->prev;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Get-function for next of angel_header.
 * @param[in] param
 *	Parameter, which is not used.
 * @param[out] p_link
 *	Next pointer.
 * @param[in] lst
 *	Angel header.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_header_next(
	void *param,
	void ***p_link,
	void *lst)
{
	struct angel_header *head =
		(struct angel_header *)lst;

	if(lst == NULL) {
		return 1;
	}

	if(p_link != NULL) {
		*p_link = (void **)&head->next;
	}

	return 0;
}


/**
 ****************************************************************
 * @brief
 *	Obtain the data pointed by angel_header.
 * @param[out] p_data
 *	Pointer to the data of the block
 * @param[in] head
 *	Angel header.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_header_data(
	void **p_data,
	struct angel_header *head)
{
	if(head == NULL) {
		return 1;
	}

	if(p_data != NULL) {
		*p_data = &head[1];
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Calculate the size of data pointed by angel_header.
 * @param[out] size
 *	Data size of the block
 * @param[in] head
 *	Angel header.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_header_size_of_data(
	int *size,
	struct angel_header *head)
{
	void *data;

	if(angel_header_data(&data, head)) {
		return 1;
	}

	if(size != NULL) {
		*size = (char *)head->next
			- (char *)data;
	}

	return 0;
}

/*
 ****************************************************************
 ****************************************************************
 */

struct angel_free_header {
	struct angel_header *backward;
	struct angel_header *forward;
};

/**
 ****************************************************************
 * @brief
 *	Get-function for prev of free angel_header.
 * @param[in] param
 *	Parameter, which is not used.
 * @param[out] p_link
 *	Prev pointer.
 * @param[in] lst
 *	Angel header, which is free.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_free_header_prev(
	void *param,
	void ***p_link,
	void *lst)
{
	struct angel_header *head;
	struct angel_free_header *free_head;

	head = (struct angel_header *)lst;
	if(angel_header_data((void **)&free_head, head)) {
		return 1;
	}

	if(p_link != NULL) {
		*p_link = (void **)&free_head->backward;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Get-function for next of free angel_header.
 * @param[in] param
 *	Parameter, which is not used.
 * @param[out] p_link
 *	 Next pointer.
 * @param[in] lst
 *	Angel header, which is free.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_free_header_next(
	void *param,
	void ***p_link,
	void *lst)
{
	struct angel_header *head;
	struct angel_free_header *free_head;

	head = (struct angel_header *)lst;
	if(angel_header_data((void **)&free_head, head)) {
		return 1;
	}

	if(p_link != NULL) {
		*p_link = (void **)&free_head->forward;
	}

	return 0;
}

struct angel_lock_header {
	struct funlist_chain_pointer *ring;
	//struct funlist_chain_pointer *link;
	void *data;
};

#define FREE(head)	((struct angel_free_header *)&((struct angel_header *)head)[1])
#define LOCK(head)	((struct angel_lock_header *)&((struct angel_header *)head)[1])
#define LOCK_DATA(head)	(void *)(&LOCK(head)->data)
#define LOCK_LINK(head)	((struct funlist_chain_pointer *)&LOCK(head)[1])

int is_lock(struct angel_header *head)
{
	//if((char *)LOCK_LINK(head) > (char *)head
	//	&& (char *)LOCK_LINK(head) < (char *)head->next)
	if((char *)LOCK(head)->data > (char *)head
		&& (char *)LOCK(head)->data < (char *)head->next)
	{
		return 1;
	}
	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Get-function for the angel ring of lock angel_header.
 * @param[out] p_ring
 *	Pointer to the angel ring of the block.
 * @param[in] head
 *	Angel header, which is lock.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_lock_header_get_ring(
	struct funlist_chain_pointer **p_ring,
	struct angel_header *head)
{
	struct angel_lock_header *lock_head;

	if(angel_header_data((void **)&lock_head, head)) {
		return 1;
	}

	if(p_ring != NULL) {
		*p_ring = lock_head->ring;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Set-function for the angel ring of lock angel_header.
 * @param[in] head
 *	Angel header, which is lock.
 * @param[in] ring
 *	Pointer to the angel ring of the block.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_lock_header_set_ring(
	struct angel_header *head,
	struct funlist_chain_pointer *ring)
{
	struct angel_lock_header *lock_head;

	if(angel_header_data((void **)&lock_head, head)) {
		return 1;
	}

	lock_head->ring = ring;

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Obtain the link pointed by lock angel_header.
 * @param[out] p_link
 *	Pointer to the link of the block.
 * @param[in] head
 *	Angel header, which is lock.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_lock_header_link(
	struct funlist_chain_pointer **p_link,
	struct angel_header *head)
{
	struct angel_lock_header *lock_head;

	if(angel_header_data((void **)&lock_head, head)) {
		return 1;
	}

	if(p_link != NULL) {
		*p_link = (struct funlist_chain_pointer *)
			&lock_head[1];
	}

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Obtain the data pointed by lock angel_header.
 * @param[out] size
 *	Pointer to the data of the block.
 * @param[in] head
 *	Angel header, which is lock.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_lock_header_data(
	void **p_data,
	struct angel_header *head)
{
	struct angel_lock_header *lock_head;

	if(angel_header_data((void **)&lock_head, head)) {
		return 1;
	}

	if(p_data != NULL) {
		*p_data = lock_head->data;
	}

	return 0;
}


int angel_lock_header_size_of_link(
	int *size,
	struct angel_header *head)
{
	struct funlist_chain_pointer *link;
	struct funlist_chain_pointer *data;

	if(angel_lock_header_link(&link, head)) {
		return 1;
	}
	if(angel_lock_header_data((void **)&data, head)) {
		return 1;
	}

	if(size != NULL) {
		*size = data - link;
	}

	return 0;
}
int angel_lock_header_size_of_data(
	int *size,
	struct angel_header *head)
{
	void *data;

	if(angel_lock_header_data((void **)&data, head)) {
		return 1;
	}

	if(size != NULL) {
		*size = (char *)head->next
			- (char *)data;
	}

	return 0;
}

#if 0
int angel_lock_header_size_of_link(
	int *size,
	struct angel_header *head)
{
	struct funlist_chain_pointer *link;
	struct funlist_chain_pointer *data;

	if(angel_lock_header_data(&link, head)) {
		return 1;
	}

	if(size != NULL) {
		*size = (char *)head->next
			- (char *)data;
		*size = data
			- (struct funlist_chain_pointer *)LOCK(head);
	}

	return 0;
}
#endif

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
			int sdata;
			int slink;

			angel_lock_header_size_of_data(&sdata, temp);
			angel_lock_header_size_of_link(&slink, temp);
			printf("\tsize, link, angel = %d, %d, %x\n",
				sdata, slink, LOCK(temp)->ring);
			for(i = 0; i < slink; ++i) {
				struct funlist_chain_pointer *link;
				void *ptr;

				angel_lock_header_link(&link, temp);
				printf("\tlink[%d](%x): prev(%x), next(%x), pointery(%x)\n",
					i, &link[i],
					link[i].prev,
					link[i].next,
					link[i].pointer);
			}
		} else {
			int sdata;

			angel_header_size_of_data(&sdata, temp);
			printf("\tsize = %d\n", sdata);
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
		NULL,
		angel_free_header_prev,
		NULL,
		angel_free_header_next,
		head);

	return 0;
}

int angel_free_header_split(
	struct angel_header *head,
	int size)
{
	struct angel_header *tail;

	tail = (struct angel_header *)
		((char *)FREE(head) + size);
	tail->next = head->next;
	head->next = tail;

	funlist_cyclic_list_init(
		NULL,
		angel_free_header_prev,
		NULL,
		angel_free_header_next,
		tail);
	funlist_cyclic_list_insert(
		NULL,
		angel_free_header_prev,
		NULL,
		angel_free_header_next,
		head,
		tail);

	return 0;
}
int angel_free_header_to_lock(
	struct angel_header *head)
{
	funlist_cyclic_list_insert(
		NULL, angel_free_header_prev,
		NULL, angel_free_header_next,
		head, head);
	LOCK(head)->ring = NULL;
	LOCK(head)->data = &LOCK(head)[1];

	return 0;
}
int angel_lock_header_extend_link(
	struct angel_header *head,
	int link)
{
	int i;
	void *new_data;

	angel_lock_header_data(&new_data, head);
	new_data = ((struct funlist_chain_pointer *)LOCK(head)->data) + link;
	if((char *)head->next < (char *)new_data) {
		return 1;
	}

	for(i = 0; i < link; ++i) {
		struct funlist_chain_pointer *chnptr;

		chnptr = &((struct funlist_chain_pointer *)LOCK(head)->data)[i];
		funlist_cyclic_list_init(
			NULL, funlist_chain_pointer_prev,
			NULL, funlist_chain_pointer_next,
			chnptr);
		chnptr->pointer = NULL;
	}
	LOCK(head)->data = new_data;

	return 0;
}

int angel_free_header_size_of_alloc(
	int *size,
	int sdata,
	int slink)
{
	if(size != NULL) {
		*size = sizeof(struct angel_lock_header) + sdata +
			sizeof(struct funlist_chain_pointer) * slink;
	}

	return 0;
}
int angel_free_header_alloc(
	struct angel_header *head,
	int sdata,
	int slink)
{
	int size;
	struct angel_header *tail;

	angel_free_header_size_of_alloc(&size, sdata, slink);
	angel_free_header_split(head, size);
	angel_free_header_to_lock(head);
	angel_lock_header_extend_link(head, slink);
}

int angel_lock_header_get_link(
	struct angel_header **p_object,
	struct angel_header *head,
	int n)
{
	struct funlist_chain_pointer *link;

	if(angel_lock_header_link(&link, head)) {
		return 1;
	}

	*p_object = link[n].pointer;

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Set-function for n-th link of angel_header.
 * @param[in] head
 *	Angel header, which is lock.
 * @param[in] n
 *	Index of links where to be set.
 * @param[in] object
 *	Object which to be set.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_lock_header_link_set(
	struct funlist_chain_pointer *link,
	int n,
	struct angel_header *object)
{
	struct angel_header *target;

/**
 ****************************************************************
 * 1.
 *	The link[n] is refreshed. The content link[n].pointer are
 *	going to be deleted. Before the delete, confirm that the
 *	link[n] is not directly pointed from the pointing object
 *	(target), that is, link[n] is not saved in the field
 *	ring. But, if nothing was formerly pointed by link[n], do
 *	nothing.
 */
	funlist_chain_pointer_get((void **)&target, link);
	if(target != NULL) {
/**
 *	When the link[n] is direct angel of the pointing object,
 *	check whether link[n] is the only one pointer that is
 *	pointing the target.
 */
		if(LOCK(target)->ring == link) {
			int is_single;

			funlist_list_is_last(
				&is_single,
				NULL,
				funlist_chain_pointer_prev,
				link,
				link);
/**
 *	If so, there would be no pointer referencing the target,
 *	then the angel of the target becomes empty. Otherwise,
 *	update the angel of the target to the NEXT of the angel
 *	ring pointer.
 */
			if(is_single) {
				angel_lock_header_set_ring(target, NULL);
			} else {
				struct funlist_chain_pointer **p_ring;

				// Angel becomes next of link[n], which is
				// relatively old reference to the target.
				funlist_chain_pointer_next(
					NULL,
					(void ***)&p_ring,
					link);
				angel_lock_header_set_ring(target, *p_ring);
			}
		}
	}
/**
 *	After completing the update of angel, kick
 *	out link[n] from the angel ring.
 */
	funlist_cyclic_list_insert(
		NULL,
		funlist_chain_pointer_prev,
		NULL,
		funlist_chain_pointer_next,
		link,
		link);

/**
 ****************************************************************
 * 2.
 *	The content of link[n] is settled. The substituded value
 *	is the input object. If the input object is empty or
 *	object is newly referred, the topology of pointer should
 * 	remain initialized, so the pointer remains to be a self
 *	loop.
 */
	funlist_chain_pointer_set(object, link);
	//link[n].pointer = object;
	if(object != NULL) {
		struct funlist_chain_pointer *object_ring;

/**
 *	If the input object is not empty and the object is
 *	already referred by other pointer, we have to add the
 *	link[n] to the existing angel ring pointers. Be careful
 *	that we locate link[n] before the angel, that is, link[n]
 *	is PREV of angel. This implies the link[n] is the latest
 *	reference to the input object. The structure of angel
 *	ring is
 * @code
 *		    ... ... ... ... ... ... ...	
 *		    V                         ^
 *		  prev   -->   angel   -->   next
 *		(latest)      (oldest)   (2nd oldest)
 *		                 ^
 *		                 |
 *		               target
 * @endcode
 */
		angel_lock_header_get_ring(&object_ring, object);
		if(object_ring != NULL) {
			// link[n] is located just before angel, which is
			// the latest reference to the object.
			funlist_cyclic_list_insert(
				NULL,
				funlist_chain_pointer_prev,
				NULL,
				funlist_chain_pointer_next,
				link,
				object_ring);

			return 0;
		}
/**
 *	On the other hand, when the object is newly referred, the
 *	angel must be registered for the first time.
 */
		angel_lock_header_set_ring(object, link);
	}

#if 0
	// No need because link[n] is initialized at the
	// last of phase 1, when link[n] is omitted.
	funlist_cyclic_list_init(
		NULL, funlist_chain_pointer_prev,
		NULL, funlist_chain_pointer_next,
		&link[n]);
#endif

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Set-function for n-th link of angel_header.
 * @param[in] head
 *	Angel header, which is lock.
 * @param[in] n
 *	Index of links where to be set.
 * @param[in] object
 *	Object which to be set.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int angel_lock_header_set_link(
	struct angel_header *head,
	int n,
	struct angel_header *object)
{
	struct funlist_chain_pointer *link;
	struct angel_header *target;

	if(angel_lock_header_link(&link, head)) {
		return 1;
	}
	if(angel_lock_header_link_set(&link[n], n, object)) {
		return 1;
	}

	return 0;
}

int angel_lock_header_migrate(
	struct angel_header *from,
	struct angel_header *to)
{
	int ret;
	int i;
	int slink;
	int sdata;
	struct funlist_chain_pointer *from_ring;
	struct funlist_chain_pointer *from_link;

	if(angel_lock_header_size_of_link(&slink, from)) {
		return 1;
	}
	if(angel_lock_header_size_of_data(&sdata, from)) {
		return 1;
	}
	if(angel_free_header_alloc(to, sdata, slink)) {
		return 1;
	}

/**
 * 1.	Initialize the links.
 */
	angel_lock_header_link(&from_link, from);
	for(i = 0; i < slink; ++i) {
		ret = angel_lock_header_set_link(
			to,
			i,
			from_link[i].pointer);
		if(ret != 0) {
			return 1;
		}
		ret = angel_lock_header_set_link(
			from,
			i,
			NULL);
		if(ret != 0) {
			return 1;
		}
	}

/**
 * 2.	Initialize the ring.
 *	All the nodes in a ring must point to TO.
 */
	angel_lock_header_get_ring(&from_ring, from);
	ret = funlist_list_map(
		NULL,				// last
		NULL,				// link_param
		funlist_chain_pointer_next,	// link
		to,				// func_param
		funlist_chain_pointer_set,	// func
		NULL,				// cond_param
		funlist_default_condition,	// cond
		from_ring,			// lst
		from_ring);			// sen
	if(ret != 0) {
		return 1;
	}
	angel_lock_header_set_ring(to, from_ring);
	angel_lock_header_set_ring(from, NULL);

	return 0;
}

int angel_lock_header_to_free(
	struct angel_header *head)
{
	int ret;
	int i;
	int slink;
	struct funlist_chain_pointer *ring;
	struct funlist_chain_pointer *from_link;

	if(angel_lock_header_size_of_link(&slink, head)) {
		return 1;
	}

	for(i = 0; i < slink; ++i) {
		ret = angel_lock_header_set_link(
			head,
			i,
			NULL);
		if(ret != 0) {
			return 1;
		}
	}

	funlist_cyclic_list_init(
		NULL,
		angel_free_header_prev,
		NULL,
		angel_free_header_next,
		head);
/*
	angel_lock_header_get_ring(&ring, head);
	ret = funlist_list_map(
		NULL,				// last
		NULL,				// link_param
		funlist_chain_pointer_prev,	// link
		to,				// func_param
		funlist_chain_pointer_set,	// func
		NULL,				// cond_param
		funlist_default_condition,	// cond
		ring,				// lst
		ring);				// sen
	if(ret != 0) {
		return 1;
	}
	angel_lock_header_set_ring(head, NULL);
*/

	return 0;
}

struct angel_header buffer[1000];
struct angel_header *bin[8];
struct angel_header *pool;

int alloc_wrapper(
	struct angel_header **p_pool,
	struct angel_header **p_head,
	int sdata,
	int slink)
{
	angel_free_header_alloc(
		*p_pool,
		sdata,
		slink);
	*p_head = *p_pool;
	*p_pool = (struct angel_header *)(*p_pool)->next;

	return 0;
}
int migrate_wrapper(
	struct angel_header **p_pool,
	struct angel_header **p_head,
	struct angel_header *head)
{
	angel_lock_header_migrate(
		head,
		*p_pool);
	*p_head = *p_pool;
	*p_pool = (struct angel_header *)(*p_pool)->next;

	return 0;
}
int free_wrapper(
	struct angel_header **p_pool,
	struct angel_header *head)
{
	angel_lock_header_to_free(head);
	funlist_cyclic_list_insert(
		NULL,
		angel_free_header_prev,
		NULL,
		angel_free_header_next,
		*p_pool,
		head);


	return 0;
}

int best_fit_alloc(
	struct angel_header **p_head,
	int sdata, int slink)
{
	int size;
/*
	angel_free_header_size_of_alloc(&size, sdata, slink);
	if(size < 32 * 8) {
		bin[size / 32]
	}
	angel_free_header_alloc(pool, sdata, slink);
	*p_head = pool;
	pool = pool->next;
*/
}

int main()
{
	struct angel_header *head1, *head2, *head3, *head4;
	struct angel_header *variable;
	int slink, sdata;

	printf("hello, world!\n");
	printf("sizeof(funlist_chain_pointer) = %d bytes\n",
		sizeof(struct funlist_chain_pointer));
	printf("sizeof(angel_header) = %d bytes\n",
		sizeof(struct angel_header));
	printf("sizeof(angel_free_header) = %d bytes\n",
		sizeof(struct angel_free_header));
	printf("sizeof(angel_lock_header) = %d bytes\n",
		sizeof(struct angel_lock_header));

	angel_free_header_init(buffer, sizeof(buffer));
/*
	funlist_cyclic_list_init(
		NULL,
		funlist_chain_pointer_prev,
		NULL,
		funlist_chain_pointer_next,
		pool);
	funlist_chain_pointer_set(buffer, pool);
*/
	pool = buffer;

	print(buffer);
	printf("\n");

	printf("################################\n"); fflush(stdout);

#if 0
	angel_free_header_alloc(pool, 1024, 8); proc = pool; pool = pool->next;
	angel_lock_header_set_link(proc, 0, proc);

	angel_free_header_alloc(pool, 0, 8);
	variable = pool;
	pool = pool->next;

	angel_free_header_alloc(pool, 8, 1);
	strcpy(LOCK_DATA(pool), "add");
	angel_lock_header_set_link(variable, 0, pool);
	pool = pool->next;

	angel_free_header_alloc(pool, 8, 1);
	strcpy(LOCK_DATA(pool), "square");
	angel_lock_header_set_link(variable, 1, pool);
	pool = pool->next;

	angel_free_header_alloc(pool, sizeof(int), 3);
	*(int *)LOCK_DATA(pool) = 0;
	pool = pool->next;

	angel_free_header_alloc(pool, sizeof(int), 3);
	*(int *)LOCK_DATA(pool) = 1;
	pool = pool->next;
#endif

	alloc_wrapper(&pool, &head1, 10, 1);
	alloc_wrapper(&pool, &head2, 20, 2);
	alloc_wrapper(&pool, &head3, 30, 3);
	angel_lock_header_set_link(head1, 0, head2);
	angel_lock_header_set_link(head2, 0, head1);
	angel_lock_header_set_link(head3, 0, head1);

	print(buffer);
	printf("################################\n"); fflush(stdout);

	migrate_wrapper(&pool, &head4, head2);
	free_wrapper(&pool, head2);

	print(buffer);
}
