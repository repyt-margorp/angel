#include <stdio.h>

struct pointer {
	struct pointer *prev;
	struct pointer *next;
#if 0
	struct header *memory;
#else
	void *memory;
#endif
};

int pointer_init(
	struct pointer *ptr)
{
	if(ptr == NULL) {
		return 1;
	}

	ptr->prev = ptr;
	ptr->next = ptr;

	return 0;
}
int pointer_is_single(
	struct pointer *ptr,
	int *res)
{
	if(ptr == NULL) {
		return 1;
	}

	if(ptr == ptr->prev) {
		*res = 1;
	} else {
		*res = 0;
	}

	return 0;
}
int pointer_set(
	struct pointer *ptr,
	void *mem)
{
	struct pointer *temp;

	if(ptr == NULL) {
		return 1;
	}

	temp = ptr;
	do {
		temp->memory = mem;
		temp = temp->next;
	} while(temp != ptr);

	return 0;
}
/**
 ****************************************************************
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
 *		  .        |           |        .
 *		  .      next_P     prev_Q      .
 *		  .        ^           |        .
 *		  .        |           |        .
 *		  .        |           V        .
 *		prev_P --> P           Q --> next_Q
 *	is converted into two loops.
 *                  ...   ...   ...   ...   ...
 *		  .                             .
 *		  .         ... ... ...         .
 *		  .        |           |        .
 *		  .      next_P <-- prev_Q      .
 *		  .                             .
 *		prev_P --> P    -->    Q --> next_Q
 ****************************************************************
 */
int pointer_insert(
	struct pointer *p,
	struct pointer *q)
{
	if(p == NULL || q == NULL) {
		return 1;
	}

	p->next->prev = q->prev;
	q->prev->next = p->next;
	p->next = q;
	q->prev = p;

	return 0;
}

struct header {
	struct header *prev;
	struct header *next;
	struct header *last;
	struct pointer *link;
	struct pointer *angel;
};

int header_link(struct header *head)
{
	int link;

	link = (struct pointer *)head->last - head->link;
	return link;
}
int header_size(struct header *head)
{
	int size;

	size = (char *)head->link - (char *)&head[1];
	return size;
}

int print(struct header *head)
{
	struct header *temp;

	temp = head;
	while(temp != NULL) {
		int i;
		int size;
		int link;

		printf("%x: prev(%x), next(%x), link(%x), last(%x), angel(%x)\n",
			temp, temp->prev, temp->next,
			temp->link, temp->last, temp->angel);

		size = header_size(temp);
		link = header_link(temp);
		printf("\tsize, link = %d, %d\n", size, link);
		for(i = 0; i < link; ++i) {
			printf("\tlink[%d](%x): prev(%x), next(%x), memory(%x)\n",
				i, &temp->link[i],
				temp->link[i].prev,
				temp->link[i].next,
				temp->link[i].memory);
		}

		temp = temp->next;
	}
}
int split(struct header *head, int size, int length)
{
	int i;
	struct pointer *link;
	struct header *tail;

	link = (char *)&head[1] + size;
	tail = (char *)link
		+ sizeof(struct pointer) * length;
	tail->link = head->link;
	tail->last = head->last;
	head->link = link;
	head->last = tail;

	for(i = 0; i < length; ++i) {
#if 0
		link[i].prev = &link[i];
		link[i].next = &link[i];
#else
		pointer_init(&link[i]);
#endif
		link[i].memory = NULL;
	}

	if(head->next != NULL) {
		head->next->prev = tail;
	}
	tail->next = head->next;
	tail->prev = head;
	head->next = tail;
}
int link_set(struct header *head, int n, struct header *object)
{
	struct pointer *temp_prev;
	struct pointer *temp_next;
	int is_single;
	struct header *target;

#if 0
	temp_prev = head->link[n].prev;
	temp_next = head->link[n].next;
	temp_prev->next = temp_next;
	temp_next->prev = temp_prev;
#else
	pointer_is_single(&head->link[n], &is_single);
	pointer_insert(&head->link[n], &head->link[n]);
#endif

	target = head->link[n].memory;
	if(target != NULL) {
		if(target->angel == &head->link[n]) {
//			if(&head->link[n] == temp_next) {
			if(is_single) {
				target->angel = NULL;
			} else {
				target->angel = temp_next;
			}
		}
	}

	head->link[n].memory = object;
	if(object != NULL) {
		struct pointer *temp;

		temp = object->angel;
		object->angel = &head->link[n];
		if(temp != NULL) {
#if 0
			head->link[n].next = temp;
			head->link[n].prev = temp->prev;
			temp->prev->next = &head->link[n];
			temp->prev = &head->link[n];
#else
			pointer_insert(&head->link[n], temp);
#endif
			return 0;
		}
	}
#if 0
	head->link[n].next = &head->link[n];
	head->link[n].prev = &head->link[n];
#else
	pointer_init(&head->link[n]);
#endif

	return 0;
}
int migrate(struct header *from, struct header *to)
{
	int i;
	int length;
	int size;
	struct pointer *temp;

	length = header_link(from);
	size = header_size(from);
	split(to, size, length);

#if 0
	temp = from->angel;
	if(temp != NULL) {
		do {
			temp->memory = to;
			temp = temp->next;
		} while(temp != from->angel);
	}
#else
	pointer_set(from->angel, to);
#endif
	to->angel = from->angel;
	from->angel = NULL;

	for(i = 0; i < length; ++i) {
		link_set(to, i, from->link[i].memory);
		link_set(from, i, NULL);
	}
}

//
//	link i  j;
//	link_set(proc, i, proc->link[j].memory);
//	in i j;
//	link_set(proc, i, proc->link[i].memory->link[j].memory);
//



struct header buffer[1000];
struct header *pool;
int main()
{
	struct header *arena, *proc;
	struct header *head1, *head2, *head3, *head4;
	printf("hello, world!\n");
	printf("sizeof(pointer) = %d\n", sizeof(struct pointer));
	printf("sizeof(header) = %d\n", sizeof(struct header));

	buffer[0].prev = NULL;
	buffer[0].next = NULL;
	buffer[0].last = &buffer[1000];
	buffer[0].link = buffer[0].last; 
	pool = buffer;
	printf("link, size = %d, %d\n",
		header_link(pool), header_size(pool));

	print(buffer);

#if 0
#define NUM (300)
	split(pool, 0, 2*NUM); arena = pool; pool = pool->next;
	{
		int i;

		for(i = 0; i <
	}
#endif

#if 0
	split(pool, 0, 10); proc = pool; pool = pool->next;
	split(pool, 1000, 0); arena = pool; pool = pool->next;
	split(arena, 0, 2);
	link_set(arena, 0, arena->next);
//	split(arena->link[0], size, length);
#endif

	split(pool, 10, 1); head1 = pool; pool = pool->next;
	split(pool, 20, 2); head2 = pool; pool = pool->next;
	split(pool, 30, 3); head3 = pool; pool = pool->next;
	link_set(head1, 0, head2);
	link_set(head2, 0, head1);
	link_set(head3, 0, head1);
	print(buffer);
	printf("################################\n");
	migrate(head2, pool); head4 = pool; pool = pool->next;
	print(buffer);
}
