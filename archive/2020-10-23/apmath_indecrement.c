/**
 ****************************************************************
 *	Arbitrary Precision Mathematics Library
 * @file
 *	apmath_indecrement.c
 * @author
 *	Ryota MURAI
 * @date
 *	2020/09/21
 * @details
 *	Increment and decrement codes.
 ****************************************************************
 */

#include "apmath.h"

/**
 ****************************************************************
 * @brief
 *	Increment an apmath base operand.
 * @param[out] result
 *	Incremented value of operand.
 * @param[out] carry_out
 *	Output carry.
 * @param[in] operand
 *	The number to be incremented.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_increment_with_base(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand)
{
	apmath_base_t temp;

	if(result == NULL) {
		return 1;
	}

	temp = operand + APMATH_BASE_ONE;
	*result = temp;

	if(carry_out == NULL) {
		return 0;
	}
//	if(temp == APMATH_BASE_ZERO) {
	if(operand == APMATH_BASE_MAX) {
		*carry_out = APMATH_BASE_ONE;
	} else {
		*carry_out = APMATH_BASE_ZERO;
	}

	return 0;
}
int apmath_base_increment_with_super(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand)
{
	apmath_super_t temp;

	if(result == NULL) {
		return 1;
	}

	temp = operand + APMATH_SUPER_ONE;
	*result = temp & APMATH_BASE_FULL;

	if(carry_out == NULL) {
		return 0;
	}
	*carry_out = temp >> APMATH_BASE_BIT_LENGTH;

	return 0;
}
int apmath_base_increment(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand)
{
#if 1
	return apmath_base_increment_with_base(
		result,
		carry_out,
		operand);
#else
	return apmath_base_increment_with_super(
		result,
		carry_out,
		operand);
#endif
}

/**
 ****************************************************************
 * @brief
 *	Decrement an apmath base operand.
 * @param[out] result
 *	Decremented value of operand.
 * @param[out] carry_out
 *	Output carry.
 * @param[in] operand
 *	The number to be decremented.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_decrement_with_base(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand)
{
	apmath_base_t temp;

	if(result == NULL) {
		return 1;
	}

	temp = operand + APMATH_BASE_MAX;
	*result = temp;

	if(carry_out == NULL) {
		return 0;
	}
//	if(temp == APMATH_BASE_MAX) {
	if(operand == APMATH_BASE_ZERO) {
		*carry_out = APMATH_BASE_MAX;
	} else {
		*carry_out = APMATH_BASE_ZERO;
	}

	return 0;
}
int apmath_base_decrement_with_super(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand)
{
	apmath_super_t temp;

	if(result == NULL) {
		return 1;
	}

	temp = operand + APMATH_SUPER_MAX;
	*result = temp & APMATH_BASE_FULL;

	if(carry_out == NULL) {
		return 0;
	}
	*carry_out = temp >> APMATH_BASE_BIT_LENGTH;

	return 0;
}
int apmath_base_decrement(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand)
{
#if 1
	return apmath_base_decrement_with_base(
		result,
		carry_out,
		operand);
#else
	return apmath_base_decrement_with_super(
		result,
		carry_out,
		operand);
#endif
}

/**
 ****************************************************************
 * @brief
 *	Increment an apmath base array operand.
 * @param[out] result
 *	Incremented value of operand.
 * @param[out] carry_out
 *	Output carry.
 * @param[in] operand
 *	The number to be incremented.
 * @param[in] length
 *	The length of operand array.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_array_increment(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *operand,
	int length)
{
	int apmath_base_array_increment_copy_forward(
		apmath_base_t *result,
		apmath_base_t *carry_out,
		apmath_base_t *operand,
		int length);
	int apmath_base_array_increment_inplace(
		apmath_base_t *array,
		apmath_base_t *carry_out,
		int length);
	apmath_base_t carry;
	int offset;

	if(result < &operand[0] || &operand[length] <= result) {
		return apmath_base_array_increment_copy_forward(
			result,
			carry_out,
			operand,
			length);
	}

	offset = result - operand;
	apmath_base_array_copy(
		&result[offset],
		&operand[offset],
		length - offset);
	apmath_base_array_increment_copy_forward(
		result,
		&carry,
		operand,
		offset);
	if(carry != APMATH_BASE_ZERO) {
		apmath_base_array_increment_inplace(
			&result[offset],
			&carry,
			length - offset);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
int apmath_base_array_increment_copy_forward(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *operand,
	int length)
{
	int i;
	apmath_base_t carry;

	if(result == NULL) {
		return 1;
	}

	carry = APMATH_BASE_ONE;
	for(i = 0; i < length; ++i)
	{
		if(carry != APMATH_BASE_ZERO) {
			break;
		}
		apmath_base_increment(
			&result[i],
			&carry,
			operand[i]);
	}

	apmath_base_array_copy(
		&result[i],
		&operand[i],
		length - i);

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
int apmath_base_array_increment_inplace(
	apmath_base_t *array,
	apmath_base_t *carry_out,
	int length)
{
	int i;
	apmath_base_t carry;

	if(array == NULL) {
		return 1;
	}

	carry = APMATH_BASE_ONE;
	for(i = 0; i < length; ++i)
	{
		if(carry == APMATH_BASE_ZERO) {
			break;
		}
		apmath_base_increment(
			&array[i],
			&carry,
			array[i]);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Decrement an apmath base array operand.
 * @param[out] result
 *	Decremented value of operand.
 * @param[out] carry_out
 *	Output carry.
 * @param[in] operand
 *	The number to be decremented.
 * @param[in] length
 *	The length of operand array.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_array_decrement(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *operand,
	int length)
{
	int apmath_base_array_decrement_copy_forward(
		apmath_base_t *result,
		apmath_base_t *carry_out,
		apmath_base_t *operand,
		int length);
	int apmath_base_array_decrement_inplace(
		apmath_base_t *array,
		apmath_base_t *carry_out,
		int length);
	apmath_base_t carry;
	int offset;

	if(result < &operand[0] || &operand[length] <= result) {
		return apmath_base_array_decrement_copy_forward(
			result,
			carry_out,
			operand,
			length);
	}

	offset = result - operand;
	apmath_base_array_copy(
		&result[offset],
		&operand[offset],
		length - offset);
	apmath_base_array_decrement_copy_forward(
		result,
		&carry,
		operand,
		offset);
	if(carry != APMATH_BASE_ZERO) {
		apmath_base_array_decrement_inplace(
			&result[offset],
			&carry,
			length - offset);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
int apmath_base_array_decrement_copy_forward(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *operand,
	int length)
{
	int i;
	apmath_base_t carry;

	if(result == NULL) {
		return 1;
	}

	carry = APMATH_BASE_MAX;
	for(i = 0; i < length; ++i)
	{
		if(carry == APMATH_BASE_ZERO) {
			break;
		}
		apmath_base_decrement(
			&result[i],
			&carry,
			operand[i]);
	}

	apmath_base_array_copy(
		&result[i],
		&operand[i],
		length - i);

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
int apmath_base_array_decrement_inplace(
	apmath_base_t *array,
	apmath_base_t *carry_out,
	int length)
{
	int i;
	apmath_base_t carry;

	if(array == NULL) {
		return 1;
	}

	carry = APMATH_BASE_MAX;
	for(i = 0; i < length; ++i)
	{
		if(carry != APMATH_BASE_ZERO) {
			break;
		}
		apmath_base_decrement(
			&array[i],
			&carry,
			array[i]);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
