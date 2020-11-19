/**
 ****************************************************************
 *	Functionalist - Functional List Library -
 * @file
 *	funlist_util.c
 * @author
 *	Ryota MURAI
 * @date
 *	2020/10/23
 * @details
 *	Utility for funlist.
 ****************************************************************
 */

#include "funlist.h"

/**
 ****************************************************************
 * @brief
 *	Get-function for link of list pointer.
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
int funlist_list_pointer_link(
	void *param,
	void ***p_link,
	void *lst)
{
	struct funlist_list_pointer *lstptr =
		(struct funlist_list_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	if(p_link != NULL) {
		*p_link = (void **)&lstptr->link;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Set-function for pointer of list pointer.
 * @param[in] ptr
 *	Pointer to be set.
 * @param[in] lst
 *	List data.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_list_pointer_set(
	void *ptr,
	void *lst)
{
	struct funlist_list_pointer *chnptr =
		(struct funlist_list_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	((struct funlist_list_pointer *)chnptr)->pointer = ptr;

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Get-function for pointer of list pointer.
 * @param[in] ptr
 *	Pointer to be set.
 * @param[in] lst
 *	List data.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_list_pointer_get(
	void **p_ptr,
	void *lst)
{
	struct funlist_list_pointer *chnptr =
		(struct funlist_list_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	if(p_ptr != NULL) {
		*p_ptr = ((struct funlist_list_pointer *)chnptr)->pointer;
	}

	return 0;
}



/**
 ****************************************************************
 * @brief
 *	Get-function for prev of chain pointer.
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
int funlist_chain_pointer_prev(
	void *param,
	void ***p_link,
	void *lst)
{
	struct funlist_chain_pointer *chnptr =
		(struct funlist_chain_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	if(p_link != NULL) {
		*p_link = (void **)&chnptr->prev;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Get-function for next of chain pointer.
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
int funlist_chain_pointer_next(
	void *param,
	void ***p_link,
	void *lst)
{
	struct funlist_chain_pointer *chnptr =
		(struct funlist_chain_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	if(p_link != NULL) {
		*p_link = (void **)&chnptr->next;
	}

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Set-function for pointer of chain pointer.
 * @param[in] ptr
 *	Pointer to be set.
 * @param[in] lst
 *	List data.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_chain_pointer_set(
	void *ptr,
	void *lst)
{
	struct funlist_chain_pointer *chnptr =
		(struct funlist_chain_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	((struct funlist_chain_pointer *)chnptr)->pointer = ptr;

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Get-function for pointer of chain pointer.
 * @param[in] ptr
 *	Pointer to be set.
 * @param[in] lst
 *	List data.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_chain_pointer_get(
	void **p_ptr,
	void *lst)
{
	struct funlist_chain_pointer *chnptr =
		(struct funlist_chain_pointer *)lst;

	if(lst == NULL) {
		return 1;
	}

	if(p_ptr != NULL) {
		*p_ptr = ((struct funlist_chain_pointer *)chnptr)->pointer;
	}

	return 0;
}
