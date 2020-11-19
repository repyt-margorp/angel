/**
 ****************************************************************
 *	Arbitrary Precision Mathematics Library
 * @file
 *	apmath_addition.c
 * @author
 *	Ryota MURAI
 * @date
 *	2020/09/21
 * @details
 *	Addition codes.
 ****************************************************************
 */

#include "apmath.h"

int apmath_base_add_with_base(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand1,
	apmath_base_t operand2,
	apmath_base_t carry_in)
{
	apmath_base_t temp;

	if(result == NULL) {
		return 1;
	}
	temp = operand1 + operand2 + carry_in;
	*result = temp;

	if(carry_out == NULL) {
		return 0;
	}
	if(carry_in == APMATH_BASE_ZERO) {
		if(temp < operand1 || temp < operand2) {
			*carry_out = APMATH_BASE_ONE;
		} else {
			*carry_out = APMATH_BASE_ZERO;
		}
	} else {
		if(temp <= operand1 || temp <= operand2) {
			*carry_out = APMATH_BASE_ONE;
		} else {
			*carry_out = APMATH_BASE_ZERO;
		}
	}

	return 0;
}
int apmath_base_add_with_super(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand1,
	apmath_base_t operand2,
	apmath_base_t carry_in)
{
	apmath_super_t temp;

	if(result == NULL) {
		return 1;
	}
	temp = operand1 + operand2 + carry_in;
	*result = temp & APMATH_BASE_FULL;

	if(carry_out == NULL) {
		return 0;
	}
	*carry_out = temp >> APMATH_BASE_BIT_LENGTH;

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Add apmath base operands.
 * @param[out] result
 *	Sum of operands.
 * @param[out] carry_out
 *	Output carry.
 * @param[in] operand1
 *	The first operand.
 * @param[in] operand1
 *	The second operand.
 * @param[in] carry_in
 *	Input carry.
 * @return
 *	Zero if success. Otherwise non-zero.
 * @details
 *	No details.
 ****************************************************************
 */
int apmath_base_add(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand1,
	apmath_base_t operand2,
	apmath_base_t carry_in)
{
#if 1
	return apmath_base_add_with_base(
		result,
		carry_out,
		operand1,
		operand2,
		carry_in);
#else
	return apmath_base_add_with_super(
		result,
		carry_out,
		operand1,
		operand2,
		carry_in);
#endif
}

int apmath_base_array_add_copy_forward(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *base_operand,
	int base_length,
	apmath_base_t *sub_operand,
	int sub_length,
	apmath_base_t carry_in)
{
	int i;
	apmath_base_t carry;

	carry = carry_in;
	for(i = 0; i < sub_length; ++i) {
		apmath_base_add(
			&result[i],
			&carry,
			base_operand[i],
			sub_operand[i],
			carry);
	}
	if(carry != APMATH_BASE_ZERO) {
		apmath_base_array_increment(
			&result[sub_length],
			&carry,
			&base_operand[sub_length],
			base_length - sub_length);
	} else {
		apmath_base_array_copy(
			&result[sub_length],
			&base_operand[sub_length],
			base_length - sub_length);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
int apmath_base_array_add_inplace(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *base_operand,
	int base_length,
	apmath_base_t *sub_operand,
	int sub_length,
	apmath_base_t carry_in)
{
	int i;
	apmath_base_t carry;

	carry = carry_in;
	for(i = 0; i < sub_length; ++i) {
		apmath_base_add(
			&result[i],
			&carry,
			base_operand[i],
			sub_operand[i],
			carry);
	}
	if(carry != APMATH_BASE_ZERO) {
		apmath_base_array_increment(
			&result[sub_length],
			NULL,
			&base_operand[sub_length],
			base_length - sub_length);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
/**
 ****************************************************************
 * @brief
 *	Add apmath base array operands.
 * @param[out] result
 *	Sum of operands.
 * @param[out] carry_out
 *      Output carry.
 * @param[in] operand1
 *	The first operand.
 * @param[in] operand1_length
 *      The length of first operand array.
 * @param[in] operand2
 *	The second operand.
 * @param[in] operand2_length
 *      The length of second operand array.
 * @return
 *      Zero if success. Otherwise non-zero.
 * @details
 *      No details.
 ****************************************************************
 */
int apmath_base_array_add(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *base_operand,
	int base_length,
	apmath_base_t *sub_operand,
	int sub_length,
	apmath_base_t carry_in)
{
	int apmath_base_array_add_copy_forward(
		apmath_base_t *result,
		apmath_base_t *carry_out,
		apmath_base_t *short_operand,
		int short_length,
		apmath_base_t *long_operand,
		int long_length,
		apmath_base_t carry_in);
	int apmath_base_array_add_inplace(
		apmath_base_t *result,
		apmath_base_t *carry_out,
		apmath_base_t *base_operand,
		int base_length,
		apmath_base_t *sub_operand,
		int sub_length,
		apmath_base_t carry_in);
	apmath_base_t carry;
	int offset;

	if(result < &base_operand[0]
		|| &base_operand[base_length] <= result)
	{
		apmath_base_t *long_operand;
		int long_length;
		apmath_base_t *short_operand;
		int short_length;

		if(base_length < sub_length) {
			long_operand = sub_operand;
			long_length = sub_length;
			short_operand = base_operand;
			short_length = base_length;
		} else {
			long_operand = base_operand;
			long_length = base_length;
			short_operand = sub_operand;
			short_length = sub_length;
		}

		return apmath_base_array_add_copy_forward(
			result,
			carry_out,
			long_operand,
			long_length,
			short_operand,
			short_length,
			carry_in);
	}

	offset = result - base_operand;
	apmath_base_array_copy(
		&result[offset],
		&base_operand[offset],
		base_length - offset);
	if(sub_length <= offset) {
		apmath_base_array_add_copy_forward(
			result,
			&carry,
			base_operand,
			sub_length,
			sub_operand,
			sub_length,
			carry_in);
		if(carry != APMATH_BASE_ZERO) {
			apmath_base_array_increment(
				&result[sub_length],
				&carry,
				&result[sub_length],
				base_length - sub_length);
		}
	} else {
		apmath_base_array_add_copy_forward(
			result,
			&carry,
			base_operand,
			offset,
			sub_operand,
			offset,
			carry_in);
		if(sub_length < base_length) {
			apmath_base_array_add_inplace(
				&result[offset],
				&carry,
				&result[offset],
				sub_length - offset,
				&sub_operand[offset],
				sub_length - offset,
				carry);
			if(carry != APMATH_BASE_ZERO) {
				apmath_base_array_increment(
					&result[sub_length],
					&carry,
					&result[sub_length],
					base_length - sub_length);
			}
		} else {
			apmath_base_array_add_inplace(
				&result[offset],
				&carry,
				&result[offset],
				base_length - offset,
				&sub_operand[offset],
				base_length - offset,
				carry);
			if(carry != APMATH_BASE_ZERO) {
				apmath_base_array_increment(
					&result[base_length],
					&carry,
					&sub_operand[base_length],
					base_length - sub_length);
			}
		}
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
