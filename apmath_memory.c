/**
 ****************************************************************
 *	Arbitrary Precision Mathematics Library
 * @file
 *	apmath_memory.c
 * @author
 *	Ryota MURAI
 * @date
 *	2020/09/21
 * @details
 *	Memory management codes.
 ****************************************************************
 */

#include "apmath.h"

/**
 ****************************************************************
 * @brief
 *	Resize the apmath base array so that the upper bytes are
 *	non-zero.
 * @param[out] length
 *	Resized length.
 * @param[in] array
 *	Target base array.
 * @param[in] max_length
 *	Max length of the target.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_array_resize(
	int *length,
	apmath_base_t *array,
	int max_length)
{
	int i;

	if(length == NULL) {
		return 1;
	}

	for(i = max_length; i > 0; --i) {
		if(array[i - 1] != APMATH_BASE_ZERO) {
			break;
		}
	}
	*length = i;

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Compare two apmath base arrays.
 * @param[out] res
 *	Negative value if left is less than right and positive
 *	value if left is greater than right. Zero if left value
 *	equals right value.
 * @param[in] array
 *	Target base array.
 * @param[in] max_length
 *	Max length of the target.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_array_compare(
	int *res,
	apmath_base_t *left,
	int left_length,
	apmath_base_t *right,
	int right_length)
{
	int apmath_base_array_compare_same_length(
		int *res,
		apmath_base_t *left,
		apmath_base_t *right,
		int length);

	if(left_length < right_length) {
		*res = -1;
		return 0;
	}
	if(left_length > right_length) {
		*res = 1;
		return 0;
	}

	return apmath_base_array_compare_same_length(
		res,
		left,
		right,
		left_length);
}
int apmath_base_array_compare_same_length(
	int *res,
	apmath_base_t *left,
	apmath_base_t *right,
	int length)
{
	int i;

	for(i = length - 1; i >= 0; --i) {
		if(left[i] < right[i]) {
			*res = -1;
			return 0;
		} else if(left[i] > right[i]) {
			*res = 1;
			return 0;
		}
	}
	*res = 0;

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Set zero in an apmath base array.
 * @param[out] array
 *	Target base array.
 * @param[in] length
 *	The length of the target.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_array_zero(
	apmath_base_t *array,
	int length)
{
	int i;

	if(array == NULL) {
		return 1;
	}

	for(i = 0; i < length; ++i) {
		array[i] = APMATH_BASE_ZERO;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Set max in an apmath base array.
 * @param[out] array
 *	Target base array.
 * @param[in] length
 *	The length of the target.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_array_max(
	apmath_base_t *array,
	int length)
{
	int i;

	if(array == NULL) {
		return 1;
	}

	for(i = 0; i < length; ++i) {
		array[i] = APMATH_BASE_MAX;
	}

	return 0;
}

/**
 ****************************************************************
 * @brief
 *	Copy an apmath base array.
 * @param[out] to
 *	Apmath base array to copy into.
 * @param[in] from
 *	Apmath base array to copy from.
 * @param[in] length
 *	The length of apmath base array.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_array_copy(
	apmath_base_t *to,
	apmath_base_t *from,
	int length)
{
	int apmath_base_array_copy_forward(
		apmath_base_t *to,
		apmath_base_t *from,
		int length);
	int apmath_base_array_copy_backward(
		apmath_base_t *to,
		apmath_base_t *from,
		int length);

	if(to == NULL || from == NULL) {
		return 1;
	}

	if(to < from) {
		return apmath_base_array_copy_forward(
			to,
			from,
			length);
	}
	if(to > from) {
		return apmath_base_array_copy_backward(
			to,
			from,
			length);
	}
	// otherwise to is the same as from, so do nothing

	return 0;
}
int apmath_base_array_copy_forward(
	apmath_base_t *to,
	apmath_base_t *from,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		to[i] = from[i];
	}

	return 0;
}
int apmath_base_array_copy_backward(
	apmath_base_t *to,
	apmath_base_t *from,
	int length)
{
	int i;

	for(i = length - 1; i >= 0; --i) {
		to[i] = from[i];
	}

	return 0;
}
