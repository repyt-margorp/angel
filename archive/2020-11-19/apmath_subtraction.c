/**
 ****************************************************************
 *	Arbitrary Precision Mathematics Library
 * @file
 *	apmath_y.c
 * @author
 *	Ryota MURAI
 * @date
 *	2020/09/21
 * @details
 *	Subtraction codes.
 ****************************************************************
 */

#include "apmath.h"

/**
 ****************************************************************
 * @brief
 *	Negate an apmath base array.
 * @param[out] to
 *	Apmath base array to negate into.
 * @param[in] to_length
 *	The length of apmath base array to negate into. 
 * @param[in] from
 *	Apmath base array to negate from.
 * @param[in] from_length
 *	The length of apmath base array to negate from.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_array_negate(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length)
{
	int apmath_base_array_negate_copy(
		apmath_base_t *to,
		apmath_base_t *from,
		int length);

	if(apmath_base_array_negate_copy(
		to,
		from,
		from_length))
	{
		return 1;
	}
	apmath_base_array_max(
		&to[from_length],
		to_length - from_length);

	return 0;
}
int apmath_base_array_negate_copy(
	apmath_base_t *to,
	apmath_base_t *from,
	int length)
{
	int apmath_base_array_negate_copy_forward(
		apmath_base_t *to,
		apmath_base_t *from,
		int length);
	int apmath_base_array_negate_copy_backward(
		apmath_base_t *to,
		apmath_base_t *from,
		int length);

	if(to <= from) {
		return apmath_base_array_negate_copy_forward(
			to,
			from,
			length);
	} else {
		return apmath_base_array_negate_copy_backward(
			to,
			from,
			length);
	}

	return 0;
}
int apmath_base_array_negate_copy_forward(
	apmath_base_t *to,
	apmath_base_t *from,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		to[i] = ~from[i];
	}

	return 0;
}
int apmath_base_array_negate_copy_backward(
	apmath_base_t *to,
	apmath_base_t *from,
	int length)
{
	int i;

	for(i = length - 1; i >= 0; --i) {
		to[i] = ~from[i];
	}

	return 0;
}

/**
 ****************************************************************
 *
 ****************************************************************
 */
int apmath_base_array_negative(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length)
{
	apmath_base_array_negate(
		to,
		to_length,
		from,
		from_length);
	apmath_base_array_increment(
		to,
		NULL,
		to,
		to_length);
}
