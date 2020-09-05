/**
 ****************************************************************
 *	Arbitrary Precision Mathematics Library
 ****************************************************************
 */

typedef char apm_base_t;
typedef long apm_super_t;

#define APM_BASE_ZERO		((apm_base_t) 0)
#define APM_BASE_ONE		((apm_base_t) 1)
#define APM_BASE_MAX		((apm_base_t) -1)
#define APM_BASE_BIT_SIZE	(8 * sizeof(apm_base_t))
#define APM_BASE_BIT_HALF_SIZE	(APM_BASE_BIT_SIZE / 2)
#define APM_BASE_FULL		(APM_BASE_MAX)
#define APM_BASE_HALF_FULL	(APM_BASE_FULL >> APM_BASE_BIT_HALF_SIZE)

int apm_base_increment(
	apm_base_t *result,
	apm_base_t *carry_out,
	apm_base_t operand)
{
	*result = operand + APM_BASE_ONE;

	if(*result == APM_BASE_ZERO) {
		*carry_out = APM_BASE_ONE;
	}
	*carry_out = APM_BASE_ZERO;

	return 0;
}

int apm_base_add(
	apm_base_t *result,
	apm_base_t *carry_out,
	apm_base_t operand1,
	apm_base_t operand2,
	apm_base_t carry_in)
{
	*result = operand1 + operand2 + carry_in;

	if(carry_in == APM_BASE_ZERO) {
		if(*result < operand1 || *result < operand2) {
			*carry_out = APM_BASE_ONE;
		} else {
			*carry_out = APM_BASE_ZERO;
		}
	} else {
		if(*result <= operand1 || *result <= operand2) {
			*carry_out = APM_BASE_ONE;
		} else {
			*carry_out = APM_BASE_ZERO;
		}
	}

	return 0;
}


/**
 ****************************************************************
 *
 ****************************************************************
 */

int apm_base_array_zero(
	apm_base_t *array,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		array[i] = APM_BASE_ZERO;
	}

	return 0;
}
int apm_base_array_max(
	apm_base_t *array,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		array[i] = APM_BASE_MAX;
	}

	return 0;
}
int apm_base_array_copy(
	apm_base_t *to,
	apm_base_t *from,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		to[i] = from[i];
	}

	return 0;
}
int apm_base_array_extend(
	apm_base_t *to,
	int to_length,
	apm_base_t *from,
	int from_length)
{
	apm_base_array_copy(
		to,
		from,
		from_length);
	apm_base_array_zero(
		&to[from_length],
		to_length - from_length);
}

/**
 ****************************************************************
 *
 ****************************************************************
 */
int apm_base_array_negate(
	apm_base_t *to,
	apm_base_t *from,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		to[i] = ~from[i];
	}

	return 0;
}

/**
 ****************************************************************
 *
 ****************************************************************
 */

int apm_base_array_increment(
	apm_base_t *operand,
	int operand_length)
{
	int i;
	apm_base_t carry;

	carry = APM_BASE_ONE;
	for(i = 0; i < operand_length && carry != APM_BASE_ZERO; ++i) {
		apm_base_increment(
			&operand[i],
			&carry,
			operand[i]);
	}

	return 0;
}

int apm_base_array_add(
	apm_base_t *result,
	apm_base_t *carry_out,
	apm_base_t *short_operand,
	int short_operand_length,
	apm_base_t *long_operand,
	int long_operand_length,
	apm_base_t carry_in)
{
	int i;
	apm_base_t carry;

	carry = carry_in;
	for(i = 0; i < short_operand_length; ++i) {
		apm_base_add(
			&result[i],
			&carry,
			short_operand[i],
			long_operand[i],
			carry);
	}
	if(carry != APM_BASE_ZERO) {
		apm_base_array_increment(
			&result[short_operand_length],
			long_operand_length - short_operand_length);
	}

	*carry_out = carry;

	return 0;
}
int apm_base_array_negative(
	apm_base_t *long_operand,
	int long_operand_length,
	apm_base_t *short_operand,
	int short_operand_length)
{
#if 1
	apm_base_array_extend(
		long_operand,
		long_operand_length,
		short_operand,
		short_operand_length);
#else
	apm_base_array_zero(
		long_operand,
		long_operand_length);
	apm_base_array_copy(
		long_operand,
		short_operand,
		short_operand_length);
#endif
	apm_base_array_negate(
		long_operand,
		long_operand,
		long_operand_length);
	apm_base_array_increment(
		long_operand,
		long_operand_length);
}
/**
 ****************************************************************
 *
 ****************************************************************
 */
int apm_base_multiply(
	apm_base_t *lower,
	apm_base_t *upper,
	apm_base_t operand1,
	apm_base_t operand2)
{
	apm_super_t temp;

	temp = (apm_super_t)operand1 * (apm_super_t)operand2;
	*lower = temp & APM_BASE_FULL;
	*upper = temp >> APM_BASE_BIT_SIZE;

	return 0;
	
}
int apm_base_array_multiply(
	apm_base_t *result,
	apm_base_t *short_operand,
	int short_operand_length,
	apm_base_t *long_operand,
	int long_operand_length)
{
	int i, j;
	int result_length;
	apm_base_t carry;

	result_length = short_operand_length + long_operand_length;
	apm_base_array_zero(
		result,
		result_length);

	for(i = 0; i < short_operand_length; ++i) {
		for(j = 0; j < long_operand_length; ++j) {
			apm_base_t carry;	// go to trush
			apm_base_t temp[2];

			apm_base_multiply(
				&temp[0],
				&temp[1],
				short_operand[i],
				long_operand[j]);
			apm_base_array_add(
				&result[i + j],
				&carry,
				temp,
				2,
				&result[i + j],
				result_length - (i + j),
				APM_BASE_ZERO);
		}
	}
}
