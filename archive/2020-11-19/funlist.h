#ifndef __FUNLIST_H__
#define __FUNLIST_H__

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
	int (*link)(void *, void ***, void *),
	void *lst,
	void *sen);
int funlist_list_is_last(
	int *res,
	void *link_param,
	int (*link)(void *, void ***, void *),
	void *lst,
	void *sen);
int funlist_list_map(
	void **last,
	void *link_param,
	int (*link)(void *, void ***, void *),
	void *func_param,
	int (*func)(void *, void *),
	void *cond_param,
	int (*cond)(void *, void *, int *),
	void *lst,
	void *sen);

/**
 ****************************************************************
 ****************************************************************
 */

int funlist_cyclic_list_init(
	void *prev_param,
	int (*prev)(void *, void ***, void *),
	void *next_param,
	int (*next)(void *, void ***, void *),
	void *clst);
int funlist_cyclic_list_insert(
	void *prev_param,
	int (*prev)(void *, void ***, void *),
	void *next_param,
	int (*next)(void *, void ***, void *),
	void *lst,
	void *rst);

/**
 ****************************************************************
 ****************************************************************
 */

struct funlist_list_pointer {
	void *pointer;
	struct funlist_list_pointer *link;
};

int funlist_list_pointer_link(
	void *param,
	void ***p_link,
	void *lst);
int funlist_list_pointer_set(
	void *ptr,
	void *lst);
int funlist_list_pointer_get(
	void **p_ptr,
	void *lst);

/**
 ****************************************************************
 ****************************************************************
 */
struct funlist_chain_pointer {
	void *pointer;
	struct funlist_chain_pointer *prev;
	struct funlist_chain_pointer *next;
};

int funlist_chain_pointer_prev(
	void *param,
	void ***p_link,
	void *lst);
int funlist_chain_pointer_next(
	void *param,
	void ***p_link,
	void *lst);

int funlist_chain_pointer_set(
	void *ptr,
	void *lst);
int funlist_chain_pointer_get(
	void **p_ptr,
	void *lst);
/**
 ****************************************************************
 ****************************************************************
 */
struct funlist_chain {
	void *list;
	void *sentinel;
};

#endif
