/**
 ****************************************************************
 *	Functionalist - Functional List Library -
 * @file
 *	funlist_list.c
 * @author
 *	Ryota MURAI
 * @date
 *	2020/10/23
 * @details
 *	Simple list codes.
 ****************************************************************
 */

#include "funlist.h"

/**
 ****************************************************************
 * @brief
 *	Initialize a list with sentinel.
 * @param (link_param)
 *	Parameter for link get-function.
 * @param (link)
 *	Link get-function.
 * @param (lst)
 *	List data.
 * @param (sen)
 *	Sentinel node.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_list_init(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *lst,
	void *sen)
{
	void **ptr_link;

	if(link(link_param, lst, &ptr_link)) {
		return 1;
	}

	*ptr_link = sen;

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Check whether the list is last.
 * @param (link_param)
 *	Parameter for link get-function.
 * @param (link)
 *	Link get-function.
 * @param (lst)
 *	List data.
 * @param (sen)
 *	Sentinel node.
 * @param (res)
 *	Result is 0 if the list is not last. Non-zero if last.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_list_is_last(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *lst,
	void *sen,
	int *res)
{
	void **ptr_link;

	if(link(link_param, lst, &ptr_link)) {
		return 1;
	}

	if(*ptr_link == sen) {
		*res = 1;
	} else {
		*res = 0;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Map a function onto all the elements in a list.
 * @param[in] (link_param)
 *	Parameter for link get-function.
 * @param[in] (link)
 *	Link get-function.
 * @param[in] (func_param)
 *	Parameter for map function.
 * @param[in] (func)
 *	Function to be mapped.
 * @param[in] (cond_param)
 *	Parameter for condition function.
 * @param[in] (cond)
 *	Condition function which determines whether to continue.
 * @param[in] (lst)
 *	List data.
 * @param[in] (sen)
 *	Sentinel node.
 * @param[out] (last)
 *	Last node.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_list_map(
	void *link_param,
	int (*link)(void *, void *, void ***),
	void *func_param,
	int (*func)(void *, void *),
	void *cond_param,
	int (*cond)(void *, void *, int *),
	void *lst,
	void *sen,
	void **last)
{
	void *temp;

	temp = lst;
	while(1) {
		int is_last;
		int condition;
		void **p_temp;

/* 1.	Map a function onto the element
 */
		if(func(func_param, temp)) {
			return 1;
		}

/* 2.	Condition whether to continue
 */
		if(cond(cond_param, temp, &condition)) {
			return 1;
		}
		if(condition) {
			break;
		}

/* 3.	Check if last.
 */
		if(funlist_list_is_last(
			link_param,
			link,
			temp,
			sen,
			&is_last))
		{
			return 1;
		}
		if(is_last) {
			break;
		}

/* 4.	Update temporarily processed node.
 */
		if(link(link_param, temp, &p_temp)) {
			return 1;
		}
		temp = *p_temp;
	}

/* 5.	Output the last list node if requested.
 */
	if(last != NULL) {
		*last = temp;
	}

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Initialize a cyclic list with sentinel.
 * @param (link_param)
 *	Parameter for link get-function.
 * @param (link)
 *	Link get-function.
 * @param (lst)
 *	List data.
 * @param (sen)
 *	Sentinel node.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_cyclic_list_init(
	void *prev_param,
	int (*prev)(void *, void *, void ***),
	void *next_param,
	int (*next)(void *, void *, void ***),
	void *clst)
{
	void **ptr_prev, **ptr_next;

	funlist_list_init(
		prev_param, prev,
		clst, clst);
	funlist_list_init(
		next_param, next,
		clst, clst);

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Concatenate two cyclic lists.
 * @param (param)
 *	Parameter for get-functions for prev and next.
 * @param (prev)
 *	Get-function for prev.
 * @param (next)
 *	Get-function for next.
 * @param (lst)
 *	Node to come before rst.
 * @param (rst)
 *	Node to come after lst.
 * @return
 *	0 for success. Otherwise non-zero.
 * @details
 *	Nodes in cyclic lists are denoted L and R. Links to be
 *	broken are
 *		... L --> next_L ...
 *		... prev_R --> R ...
 *	and those to be constructed are
 *		... L --> R ...
 *		... prev_R --> next_L ...
 *	When two ditinct loops
 *		    ...  next_L   prev_R  ...
 *		  .        ^         |        .
 *		  .        |         |        .
 *		  .        |         V        .
 *		prev_L --> L         R --> next_R
 *	are conatenated, the result is
 *		  ... next_L <-- prev_R ...
 *		  .                       .
 *		  .                       .
 *		  .                       .
 *		prev_L --> L --> R --> next_R
 *	In constrast, a single loop structure
 *                  ...   ...   ...   ...   ...
 *		  .                             .
 *		  .         ... ... ...         .
 *		  .        ^           |        .
 *		  .      next_L     prev_R      .
 *		  .        |           V        .
 *		prev_L --> L           R --> next_R
 *	is converted into two loops.
 *                  ...   ...   ...   ...   ...
 *		  .                             .
 *		  .         ... ... ...         .
 *		  .        ^           V        .
 *		  .      next_L <-- prev_R      .
 *		  .                             .
 *		prev_L --> L    -->    R --> next_R
 ****************************************************************
 */
int funlist_cyclic_list_insert(
	void *prev_param,
	int (*prev)(void *, void *, void ***),
	void *next_param,
	int (*next)(void *, void *, void ***),
	void *lst,
	void *rst)
{
	void **lst_prev, **lst_next;
	void **rst_prev, **rst_next;
	void **lst_next_prev, **rst_prev_next;

/**
 * 1.	Get the fields.
 */
	if(prev(prev_param, lst, &lst_prev)) {
		return 1;
	}
	if(next(next_param, lst, &lst_next)) {
		return 1;
	}
	if(prev(prev_param, rst, &rst_prev)) {
		return 1;
	}
	if(next(next_param, rst, &rst_next)) {
		return 1;
	}
	if(prev(prev_param, *lst_next, &lst_next_prev)) {
		return 1;
	}
	if(next(next_param, *rst_prev, &rst_prev_next)) {
		return 1;
	}

/**
 * 2.	L->next is reffered and substituted.
 *	L->next->prev is substituted.
 *	R->prev is reffered and substituted.
 *	R->prev->next is substituted.
 *	Reffered pointer variables must be read firstly. 
 *	In other words, only substituted pointer variables
 *	should be substituted in advance.
 */
	*lst_next_prev = *rst_prev;	// lst->next->prev = rst->prev;
	*rst_prev_next = *lst_next;	// rst->prev->next = lst->next;
	*lst_next = rst;		// lst->next = rst;
	*rst_prev = lst;		// rst->prev = lst;

	return 0;
}
