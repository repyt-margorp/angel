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
 *	Get-function for prev of cyclic pointer.
 * @param[in] (param)
 *	Parameter, not used.
 * @param (lst)
 *	List data.
 * @param[out] (p_link)
 *	Prev pointer.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_cyclic_pointer_prev(
	void *param,
	void *lst,
	void ***p_link)
{
	struct funlist_cyclic_pointer *cycptr =
		(struct funlist_cyclic_pointer *)lst;

	if(lst == NULL || p_link == NULL) {
		return 1;
	}

	*p_link = (void **)&cycptr->prev;

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Get-function for next of cyclic pointer.
 * @param[in] (param)
 *	Parameter, not used.
 * @param (lst)
 *	List data.
 * @param[out] (p_link)
 *	Prev pointer.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_cyclic_pointer_next(
	void *param,
	void *lst,
	void ***p_link)
{
	struct funlist_cyclic_pointer *cycptr =
		(struct funlist_cyclic_pointer *)lst;

	if(lst == NULL || p_link == NULL) {
		return 1;
	}

	*p_link = (void **)&cycptr->next;

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Get-function for prev of cyclic pointer.
 * @param[in] (param)
 *	Parameter, not used.
 * @param (lst)
 *	List data.
 * @param[out] (p_link)
 *	Prev pointer.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_cyclic_pointer_set(
	void *mem,
	void *lst)
{
	struct funlist_cyclic_pointer *cycptr =
		(struct funlist_cyclic_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	((struct funlist_cyclic_pointer *)cycptr)->pointer = mem;

	return 0;
}
