#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "apmath.h"

/**
 ****************************************************************
 */
int list_init(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *ptr,
	void *sen)
{
	void **ptr_link;

	if(link(link_param, ptr, &ptr_link)) {
		return 1;
	}

	*ptr_link = sen;

	return 0;
}
int list_is_terminated(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *ptr,
	void *sen,
	int *res)
{
	void **ptr_link;

	if(link(link_param, ptr, &ptr_link)) {
		return 1;
	}

	if(*ptr_link == sen) {
		*res = 1;
	} else {
		*res = 0;
	}

	return 0;
}
int list_function_map(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *func_param,
	int (*func)(void *, void *),
	void *ptr,
	void *sen)
{
	void *temp;
	int is_terminated;

	temp = ptr;
	while(1) {
		void **p_temp;

		if(func(func_param, temp)) {
			return 1;
		}
		if(link(link_param, temp, &p_temp)) {
			return 1;
		}
		temp = *p_temp;

		if(list_is_terminated(
			link_param, link, ptr, sen,
			&is_terminated))
		{
			return 1;
		}
		if(is_terminated) {
			break;
		}
	}

	return 0;
}

int cyclic_list_init(
	void *prev_param,
	int (*prev)(void *, void *, void ***),
	void *next_param,
	int (*next)(void *, void *, void ***),
	void *ptr)
{
	void **ptr_prev, **ptr_next;

	list_init(
		prev_param, prev,
		ptr, ptr);
	list_init(
		next_param, next,
		ptr, ptr);

	return 0;
}
/**
 ****************************************************************
 * @fn
 *	Two loops of pointers are concatenated. Nodes in loops
 *	are denoded P and Q. Links to be broken are
 *		... P --> next_P ...
 *		... prev_Q --> Q ...
 *	and those to be constructed are
 *		... P --> Q ...
 *		... prev_Q --> next_P ...
 *	When two ditinct loops
 *		    ...  next_P   prev_Q  ...
 *		  .        ^         |        .
 *		  .        |         |        .
 *		  .        |         V        .
 *		prev_P --> P         Q --> next_Q
 *	are conatenated, the result is
 *		  ... next_P <-- prev_Q ...
 *		  .                       .
 *		  .                       .
 *		  .                       .
 *		prev_P --> P --> Q --> next_Q
 *	In constrast, a single loop structure
 *                  ...   ...   ...   ...   ...
 *		  .                             .
 *		  .         ... ... ...         .
 *		  .        ^           |        .
 *		  .      next_P     prev_Q      .
 *		  .        |           V        .
 *		prev_P --> P           Q --> next_Q
 *	is converted into two loops.
 *                  ...   ...   ...   ...   ...
 *		  .                             .
 *		  .         ... ... ...         .
 *		  .        ^           V        .
 *		  .      next_P <-- prev_Q      .
 *		  .                             .
 *		prev_P --> P    -->    Q --> next_Q
 * @param (param)
 *	Parameter for get-functions for prev and next.
 * @param (prev)
 *	Get-function for prev.
 * @param (next)
 *	Get-function for next.
 * @param (ptr)
 *	Node to come before qtr.
 * @param (1tr)
 *	Node to come after ptr.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int cyclic_list_insert(
	void *prev_param,
	int (*prev)(void *, void *, void ***),
	void *next_param,
	int (*next)(void *, void *, void ***),
	void *ptr,
	void *qtr)
{
	void **ptr_prev, **ptr_next;
	void **qtr_prev, **qtr_next;
	void **ptr_next_prev, **qtr_prev_next;

/* 1.	Get the fields.
 */
	if(prev(prev_param, ptr, &ptr_prev)) {
		return 1;
	}
	if(next(next_param, ptr, &ptr_next)) {
		return 1;
	}
	if(prev(prev_param, qtr, &qtr_prev)) {
		return 1;
	}
	if(next(next_param, qtr, &qtr_next)) {
		return 1;
	}
	if(prev(prev_param, *ptr_next, &ptr_next_prev)) {
		return 1;
	}
	if(next(next_param, *qtr_prev, &qtr_prev_next)) {
		return 1;
	}

/* 2.	P->next is reffered and substituted.
 *	P->next->prev is substituted.
 *	Q->prev is reffered and substituted.
 *	Q->prev->next is substituted.
 *	Reffered pointer variables must be read firstly. 
 *	In other words, only substituted pointer variables
 *	should be substituted in advance.
 */
	*ptr_next_prev = *qtr_prev;	// ptr->next->prev = qtr->prev;
	*qtr_prev_next = *ptr_next;	// qtr->prev->next = ptr->next;
	*ptr_next = qtr;		// ptr->next = qtr;
	*qtr_prev = ptr;		// qtr->prev = ptr;

	return 0;
};

struct pointer {
	struct pointer *prev;
	struct pointer *next;
	void *memory;
};
int pointer_prev(
	void *param,
	void *ptr,
	void ***p_prev)
{
	if(ptr == NULL || p_prev == NULL) {
		return 1;
	}

	*p_prev = (void **)&((struct pointer *)ptr)->prev;

	return 0;
}
int pointer_next(
	void *param,
	void *ptr,
	void ***p_next)
{
	if(ptr == NULL || p_next == NULL) {
		return 1;
	}

	*p_next = (void **)&((struct pointer *)ptr)->next;

	return 0;
}

int pointer_set(
	void *mem,
	void *ptr)
{
	if(ptr == NULL) {
		return 1;
	}

	((struct pointer *)ptr)->memory = mem;

	return 0;
}

/**
 ****************************************************************
 */

struct header {
	struct header *prev;
	struct header *next;
	int flag;
};
struct used {
	struct pointer *link;
	struct pointer *angel;
};
struct freed {
	struct header *backward;
	struct header *forward;
};
#define USED(head)	((struct used *)&((struct header *)head)[1])
#define USED_DATA(head)	(void *)(&USED(head)[1])
#define USED_LINK(head)	(USED(head)->link)
#define FREED(head)	((struct freed *)&((struct header *)head)[1])

int header_prev(
	void *param,
	void *head,
	void ***p_prev)
{
	if(head == NULL || p_prev == NULL) {
		return 1;
	}

	*p_prev = (void **)&((struct header *)head)->prev;

	return 0;
}
int header_next(
	void *param,
	void *head,
	void ***p_next)
{
	if(head == NULL || p_next == NULL) {
		return 1;
	}

	*p_next = (void **)&((struct header *)head)->next;

	return 0;
}
int freed_header_prev(
	void *param,
	void *head,
	void ***p_prev)
{
	if(head == NULL || p_prev == NULL) {
		return 1;
	}

	*p_prev = (void **)&FREED(head)->backward;

	return 0;
}
int freed_header_next(
	void *param,
	void *head,
	void ***p_next)
{
	if(head == NULL || p_next == NULL) {
		return 1;
	}

	*p_next = (void **)&FREED(head)->forward;

	return 0;
}

int header_size(struct header *head)
{
	int size;

	size = (char *)head->next - (char *)&head[1];

	return size;
}

int is_used(struct header *head)
{
	if((char *)USED(head)->link > (char *)head
		&& (char *)USED(head)->link < (char *)head->next)
	{
		return 1;
	}
	return 0;
}

int used_header_link(struct header *head)
{
	int link;

	link = (struct pointer *)head->next - USED_LINK(head);

	return link;
}
int used_header_size(struct header *head)
{
	int size;

	size = (char *)USED(head)->link
		- (char *)USED_DATA(head);

	return size;
}

int print(struct header *head)
{
	struct header *temp;

	temp = head;
	while(temp != NULL) {
		int i;
		int use;

		use = is_used(temp);
		if(use) {
			printf("USED");
		} else {
			printf("FREED");
		}
		printf("(%x): prev(%x), next(%x), flag(%x)\n",
			temp, temp->prev, temp->next, temp->flag);

		if(use) {
			int size;
			int link;

			size = used_header_size(temp);
			link = used_header_link(temp);
			printf("\tsize, link, angel = %d, %d, %x\n",
				size, link, USED(temp)->angel);
			for(i = 0; i < link; ++i) {
				printf("\tlink[%d](%x): prev(%x), next(%x), memory(%x)\n",
					i, &USED(temp)->link[i],
					USED(temp)->link[i].prev,
					USED(temp)->link[i].next,
					USED(temp)->link[i].memory);
			}
		} else {
			int size;

			size = header_size(temp);
			printf("\tsize = %d\n", size);
			printf("\tbackward(%x), forward(%x)\n",
				FREED(temp)->backward,
				FREED(temp)->forward);
		}
		temp = temp->next;
	}
}

/**
 ****************************************************************
 */

int header_init(struct header *head, int size)
{
	head->prev = NULL;
	head->next = (struct header *)((char *)head + size);
	cyclic_list_init(
		NULL, freed_header_prev,
		NULL, freed_header_next,
		head);
}

int header_trim(struct header *head, int size)
{
	struct header *tail;

	tail = (struct header *)((char *)&head[1] + size);
	tail->next = head->next;
	head->next = tail;

	cyclic_list_insert(
		NULL, freed_header_prev,
		NULL, freed_header_next,
		head, head);

	cyclic_list_init(
		NULL, freed_header_prev,
		NULL, freed_header_next,
		tail);
}

int header_split(struct header *head, int size, int length)
{
	int i;
	struct pointer *link;
	struct header *tail;

	cyclic_list_insert(
		NULL, freed_header_prev,
		NULL, freed_header_next,
		head, head);

	link = (struct pointer *)((char *)USED_DATA(head) + size);

	header_trim(head, sizeof(struct used) + size +
		sizeof(struct pointer) * length);

	USED(head)->link = link;
	USED(head)->angel = NULL;

	for(i = 0; i < length; ++i) {
		cyclic_list_init(
			NULL, pointer_prev,
			NULL, pointer_next,
			&link[i]);
		link[i].memory = NULL;
	}
}

/**
 ****************************************************************
 * @fn
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
int used_header_set_link(struct header *head, int n, struct header *object)
{
	int is_single;
	struct header *target;

/* 1.	The link[n] is refreshed. The content link[n].memory are
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
	target = USED(head)->link[n].memory;
	if(target != NULL) {
		if(USED(target)->angel == &USED(head)->link[n]) {
			list_is_terminated(NULL,
				pointer_prev,
				&USED(head)->link[n],
				&USED(head)->link[n],
				&is_single);

			if(is_single) {
				USED(target)->angel = NULL;
			} else {
				// Angel becomes next of link[n], which is
				// relatively old reference to the target.
				USED(target)->angel =
					USED(head)->link[n].next;
			}
		}
	}
	cyclic_list_insert(
		NULL, pointer_prev,
		NULL, pointer_next,
		&USED(head)->link[n],
		&USED(head)->link[n]);

/* 2.	The content of link[n] is settled. The substituded
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
	USED(head)->link[n].memory = object;
	if(object != NULL) {
		if(USED(object)->angel != NULL) {
			// link[n] is located just before angel, which is
			// the latest reference to the object.
			cyclic_list_insert(
				NULL, pointer_prev,
				NULL, pointer_next,
				&USED(head)->link[n],
				USED(object)->angel);
			return 0;
		}
		USED(object)->angel = &USED(head)->link[n];
	}
#if 0
	// No need because link[n] is initialized at the
	// last of phase 1, when link[n] is omitted.
	cyclic_list_init(
		NULL, pointer_prev,
		NULL, pointer_next,
		&USED(head)->link[n]);
#endif

	return 0;
}

int migrate(
	struct header *from,
	struct header *to)
{
	int i;
	int length;
	int size;
	struct pointer *temp;

	length = used_header_link(from);
	size = used_header_size(from);
	header_split(to, size, length);

	list_function_map(
		NULL, pointer_prev,
		to, pointer_set,
		USED(from)->angel,
		USED(from)->angel);

	USED(to)->angel = USED(from)->angel;
	USED(from)->angel = NULL;

	for(i = 0; i < length; ++i) {
		used_header_set_link(to, i, USED(from)->link[i].memory);
		used_header_set_link(from, i, NULL);
	}
}

int number_to(
	char *from,
	int len,
	char *to,
	int p_len)
{
	int i;

	for(i = 0; i < len; ++i) {
//		from->
	}
}
int instruction_fetch(struct header *head)
{
	int i;

	switch(*(char *)USED_DATA(head)) {
	case 0x00: // NOP: nop
		break;
	case 0x01: // NEW: new x
		break;
	case 0x02: // LOAD: lode i j k
		break;
	case 0x03: // STORE: store i j k
		break;
	default:
		break;
	}
}

//
//	LOAD: load i j k;
//	set_link(proc, i, proc->link[j].memory->link[j].memory);
//	STORE: store i j k;
//	set_link(proc, proc->link[j].memory->link[k].memory, i);
//

struct header buffer[1000];
struct header *pool;

int mul(struct header *pool)
{
	header_split(pool, sizeof(int), 3);
	*(int *)USED_DATA(pool) = 0;
	pool = pool->next;

	header_split(pool, sizeof(int), 3);
	*(int *)USED_DATA(pool) = 1;
	pool = pool->next;
}
int natural_number(struct header **p_pool)
{
	int len;
	int block;
	apmath_base_t dig;
	apmath_base_t buf[16];

	apmath_base_array_zero(buf, 16);
	while(1) {
		int c;
		apmath_base_t ten = 10;

		c = getchar();
		if(c == ' ') {
			break;
		}
		dig = c - '0';
		apmath_base_array_resize(
			buf, 16, &len);
		memmove(&buf[1], buf, len);
//		printf("OK1:\t");
//		print_number(buf, 16);
		apmath_base_array_multiply(
			buf, &buf[len],
			&buf[1], len,
			&ten, 1);
//		printf("OK2:\t");
//		print_number(buf, 16);
		apmath_base_array_resize(
			buf, len + 1, &len);
		apmath_base_array_add(
			buf, &buf[len],
			&dig, 1,
			buf, len,
			APMATH_BASE_ZERO);
//		printf("OK3:\t");
//		print_number(buf, 16);
	}

	apmath_base_array_resize(buf, len + 1, &len);
	block = sizeof(int) + sizeof(apmath_base_t) * ((8*len + 7)/8);
	header_split(*p_pool, block, 3);
	apmath_base_array_zero(USED_DATA(*p_pool), block);
	*(int *)USED_DATA(*p_pool) = len;
	memcpy(&((int *)USED_DATA(*p_pool))[1], buf, sizeof(apmath_base_t) * len);
	*p_pool = (*p_pool)->next;
}

int main()
{
	struct header *arena, *proc;
	struct header *head1, *head2, *head3, *head4;
	struct header *variable;
	printf("hello, world!\n");
	printf("sizeof(pointer) = %d\n", sizeof(struct pointer));
	printf("sizeof(header) = %d\n", sizeof(struct header));

	header_init(buffer, sizeof(buffer));

	pool = buffer;
	printf("link, size = %d, %d\n",
		used_header_link(pool), used_header_size(pool));

	print(buffer);
	printf("################################\n"); fflush(stdout);

#if 0
	header_split(pool, 1024, 8); proc = pool; pool = pool->next;
	used_header_set_link(proc, 0, proc);
#endif

	header_split(pool, 0, 8);
	variable = pool;
	pool = pool->next;

	header_split(pool, 8, 1);
	strcpy(USED_DATA(pool), "add");
	used_header_set_link(variable, 0, pool);
	pool = pool->next;

	header_split(pool, 8, 1);
	strcpy(USED_DATA(pool), "square");
	used_header_set_link(variable, 1, pool);
	pool = pool->next;

	header_split(pool, sizeof(int), 3);
	*(int *)USED_DATA(pool) = 0;
	pool = pool->next;

	header_split(pool, sizeof(int), 3);
	*(int *)USED_DATA(pool) = 1;
	pool = pool->next;

//	header_split(pool, 10, 1); head1 = pool; pool = pool->next;
//	header_split(pool, 20, 2); head2 = pool; pool = pool->next;
//	header_split(pool, 30, 3); head3 = pool; pool = pool->next;
//	used_header_set_link(head1, 0, head2);
//	used_header_set_link(head2, 0, head1);
//	used_header_set_link(head3, 0, head1);

	print(buffer);
	printf("################################\n"); fflush(stdout);

//	migrate(head2, pool); head4 = pool; pool = pool->next;
//	print(buffer);
	natural_number(&pool);
}
