#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>

/**
 ****************************************************************
 ****************************************************************
 */
int funlist_default_condition(
	void *param,
	void *lst,
	int *res);

/**
 ****************************************************************
 ****************************************************************
 */

int funlist_list_init(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *lst,
	void *sen);
int funlist_list_is_last(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *lst,
	void *sen,
	int *res);
int funlist_list_map(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *func_param,
	int (*func)(void *, void *),
	void *cond_param,
	int (*cond)(void *, void *, int *),
	void *lst,
	void *sen,
	void **last);

/**
 ****************************************************************
 ****************************************************************
 */
int funlist_cyclic_list_init(
	void *prev_param,
	int (*prev)(void *, void *, void ***),
	void *next_param,
	int (*next)(void *, void *, void ***),
	void *clst);
int funlist_cyclic_list_insert(
	void *prev_param,
	int (*prev)(void *, void *, void ***),
	void *next_param,
	int (*next)(void *, void *, void ***),
	void *lst,
	void *rst);

/**
 ****************************************************************
 ****************************************************************
 */
struct funlist_cyclic_pointer {
	struct funlist_cyclic_pointer *prev;
	struct funlist_cyclic_pointer *next;
	void *pointer;
};

int funlist_cyclic_pointer_prev(
	void *param,
	void *lst,
	void ***p_link);
int funlist_cyclic_pointer_next(
	void *param,
	void *lst,
	void ***p_link);
int funlist_cyclic_pointer_set(
	void *mem,
	void *lst);

#endif
