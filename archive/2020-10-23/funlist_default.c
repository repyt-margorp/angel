/**
 ****************************************************************
 *	Functional List Library
 * @file
 *	funlist_default.c
 * @author
 *	Ryota MURAI
 * @date
 *	2020/10/23
 * @details
 *	Default functions.
 ****************************************************************
 */

#include "funlist.h"

/**
 ****************************************************************
 * @fn
 *	Default condition for list_map that do not at all
 *	intervene.
 * @param[in] (param)
 *	Parameter for link get-function.
 * @param[in] (lst)
 *	Link get-function.
 * @param[in] (res)
 *	Sentinel node.
 * @return
 *	0 for success. Otherwise non-zero.
 ****************************************************************
 */
int funlist_default_condition(
	void *param,
	void *lst,
	int *res)
{
	*res = 0;

	return 0;
}

