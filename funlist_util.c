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
 * @param[in] param
 *	Parameter, which is not used.
 * @param[out] p_link
 *	Prev pointer.
 * @param[in] lst
 *	List data.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_cyclic_pointer_prev(
	void *param,
	void ***p_link,
	void *lst)
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
 * @param[in] param
 *	Parameter, which is not used.
 * @param[out] p_link
 *	Next pointer.
 * @param[in] lst
 *	List data.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_cyclic_pointer_next(
	void *param,
	void ***p_link,
	void *lst)
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
 *	Set-function for pointer of cyclic pointer.
 * @param[in] ptr
 *	Pointer to be set.
 * @param[in] lst
 *	List data.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_cyclic_pointer_set(
	void *ptr,
	void *lst)
{
	struct funlist_cyclic_pointer *cycptr =
		(struct funlist_cyclic_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	((struct funlist_cyclic_pointer *)cycptr)->pointer = ptr;

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Get-function for pointer of cyclic pointer.
 * @param[in] ptr
 *	Pointer to be set.
 * @param[in] lst
 *	List data.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_cyclic_pointer_get(
	void **p_ptr,
	void *lst)
{
	struct funlist_cyclic_pointer *cycptr =
		(struct funlist_cyclic_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	if(p_ptr != NULL) {
		*p_ptr = ((struct funlist_cyclic_pointer *)cycptr)->pointer;
	}

	return 0;
}
