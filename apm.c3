#include <stdio.h>
#include <string.h>

/**
 ****************************************************************
 *	Arbitrary Precision Mathematics Library
 ****************************************************************
 */

typedef unsigned char apm_base_t;
typedef unsigned long apm_super_t;

#define APM_BASE_ZERO		((apm_base_t) 0)
#define APM_BASE_ONE		((apm_base_t) 1)
#define APM_BASE_MAX		((apm_base_t) -1)
#define APM_BASE_BIT_SIZE	(8 * sizeof(apm_base_t))
#define APM_BASE_BIT_HALF_SIZE	(APM_BASE_BIT_SIZE / 2)
#define APM_BASE_FULL		(APM_BASE_MAX)
#define APM_BASE_HALF_FULL	(APM_BASE_FULL >> APM_BASE_BIT_HALF_SIZE)

void print_number(
	apm_base_t *array,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		printf("%02x ", array[i]);
	}
	//printf("(%u)\n", *(unsigned short *)array);
	printf("(%u)\n", *(unsigned int *)array);
}
int apm_base_array_resize(
	apm_base_t *array,
	int max_length,
	int *length)
{
	int i;

	for(i = max_length; i > 0; --i) {
		if(array[i - 1] != 0) {
			*length = i;
			return 0;
		}
	}
	*length = 1;

	return 0;
}
int apm_base_array_compare(
	int *res,
	apm_base_t *left,
	int left_length,
	apm_base_t *right,
	int right_length)
{
	int i;

	if(left_length < right_length) {
		*res = -1;
	} else if(left_length == right_length) {
		for(i = left_length - 1; i >= 0; --i) {
			if(left[i] < right[i]) {
				*res = -1;
				return 0;
			} else if(left[i] > right[i]) {
				*res = 1;
				return 0;
			}
		}
		*res = 0;
	} else {
		*res = 1;
	}

	return 0;
}

int apm_base_increment(
	apm_base_t *result,
	apm_base_t *carry_out,
	apm_base_t operand)
{
	*result = operand + APM_BASE_ONE;

	if(*result == APM_BASE_ZERO) {
		*carry_out = APM_BASE_ONE;
	} else {
		*carry_out = APM_BASE_ZERO;
	}

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
	apm_base_t *to,
	int to_length,
	apm_base_t *from,
	int from_length)
{
#if 1
	apm_base_array_extend(
		to,
		to_length,
		from,
		from_length);
#else
	apm_base_array_zero(
		to,
		to_length);
	apm_base_array_copy(
		to,
		from,
		from_length);
#endif
	apm_base_array_negate(
		to,
		to,
		to_length);
	apm_base_array_increment(
		to,
		to_length);
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
		long_operand_length);

	for(i = 0; i < short_operand_length; ++i) {
		apm_base_t temp_carry;
		apm_base_t mult[2];
		apm_base_t carry[2];

		carry[0] = 0;
		carry[1] = 0;
		for(j = 0; j < long_operand_length - 1; ++j) {
			apm_base_multiply(
				&mult[0],
				&mult[1],
				short_operand[i],
				long_operand[j]);
			apm_base_array_add(
				&result[i + j],
				&temp_carry,
				mult,
				2,
				&result[i + j],
				2,
				carry[0]);
			carry[0] = carry[1];
			carry[1] = temp_carry;
		}
		apm_base_multiply(
			&mult[0],
			&mult[1],
			short_operand[i],
			long_operand[j]);
// next line must come right after multiply
// since this may break the memory of lowest byte
// of current multiplication
		result[long_operand_length + i] = carry[1];
		apm_base_array_add(
			&result[i + j],
			&temp_carry,
			mult,
			2,
			&result[i + j],
			2,
			carry[0]);
	}
}
/**
 ****************************************************************
 *		Newton inverse is calculated along aproximating
 *	the root of the next function equation.
 *		0 = f(x) = C/x - A
 *	where C is quite a large number and A is the number whose
 *	inverse is to be computed. Aim is to compute the nearest
 *	shot B that satisfies AB = C. The derivative of the
 *	target function is
 *		f'(x) = -C/x^2
 *	Updates of newton approximation is expressed
 *		x_(n+1) = x_n - f(x_n)/f'(x_n)
 *		        = x_n - (C/(x_n) - A)/(-C/(x_n)^2)
 *		        = x_n + x_n - A(x_n)^2/C
 *		        = 2 * x_n - A(x_n)^2/C
 *	We do this update by multi-precision natural number
 *	calculation. If C is 2^n, the division on C is realized
 *	by shift calculation or even ignoring least significant
 *	bytes, which fairly shorten the computation and make the
 *	algorithm feasible.
 *
 *		In the algorithm below, series x_n is 'to', and
 *	A is 'from'. C is selected appropriately according to the
 *	length of input 'from' and 'to'. The byte length of C is
 *	the sum of the length of 'from' and 'to'. For example,
 *	'from' has 4 bytes and 'to' has 8 bytes, C has 12 bytes,
 *	and C = 'base'^(12-1).
 *
 *		Let F, T and b be 'from', 'to' and base
 *	respectively. Also tr(x) is an appropriate byte trucation
 *	of x, so tr(x) excludes number of least significant bytes
 *	and here tr(x)*b^n is nearly x. Using these notations,
 *	the evaluation of F*T truncated at (f-1) is
 *		tr(F*T)*b^(f-1) <= F*T < (tr(F*T) + 1)*b^(f-1)
 *	where f and t are byte lengths of F and T. Then multiply
 *	every term in the inequality above by T. This yields
 *	F*T^2 evaluation.
 *		tr(F*T)*T*b^(f-1) <= F*T^2 < (tr(F*T) + 1)*T*b^(f-1)
 *		                           = (tr(F*T)*T + T)*b^(f-1)
 *
 *		Since F times T yields (f+t) bytes results,
 *	tr(F*T) is (t+1) (= (f+t) - (f-1)) bytes. Accordingly
 *	tr(F*T)*T is (2t+1) bytes. We are not interested in the
 *	lower (f+t-1) bytes of F*T^2. Thus from the above
 *	evaluation on F*T^2, another t (= (f+t-1) - (f-1)) bytes
 *	are to be ignored from upper bytes of F*T^2, which is
 *	tr(F*T)*T or (tr(F*T)*T + T). How different are tr(F*T)*T
 *	and (tr(F*T)*T + T)? Recall that T is less than b^t
 *	(T < b^t), so
 *		tr(F*T)*T <= tr(F*T)*T + T < tr(F*T)*T + b^t
 *	Therefore, the whole evaluation of F*T^2 is bound above.
 *		F*T^2 < (tr(F*T)*T + b^t)*b^(f-1)
 *
 *		To calculate upper bytes of F*T^2, we only need
 *	to truncate tr(F*T)*T and (tr(F*T)*T + b^t) at t. But
 *	this calculation is applying tr again for tr(F*T)*T,
 *	namely tr(tr(F*T)*T), and decide where to add 1 or not.
 *	In other words, F*T^2, which is (f+2t) bytes, truncated
 *	at (f+t-1) yields (t+1) bytes result tr(tr(F*T)*T).
 ****************************************************************
 *
 *		It is expected that t > f and f >= 1. Thus,
 *	(t-f+1) < t always follows. When F*T is calculated, the
 *	total length of the product is (t+f). This is saved
 *	from the address (t-f+1) of the buffer. Then, truncating
 *	first (f-1) bytes is only to ignore data before the
 *	address t. Truncated data, which is tr(F*T), is denoted
 *	by star(*) below.
 *		ADDR: 0        (t-f+1)        t           2t 2t+1
 *		      V           V           V           V   V
 *		      +---+---+---+---+---+---+---+---+---+---+
 *		BUF   |   |  ...  | X |  ...  | X*|  ...* | X*|
 *		      +---+---+---+---+---+---+---+---+---+---+
 *
 *		Next calculate tr(F*T)*T. This is achieved by
 *	multiplication of temporary 'to' and the buffer which was
 *	notated by star(*) above. Total length of tr(F*T)*T needs
 *	(2t+1) bytes of buffers. But for in-place multiplication,
 *	at least t bytes must be preserved on the execution of
 *	operation on length t and (t+1) operands. This explains
 *	why we avoided to use some first bytes in the buffer. The
 *	length of product tr(F*T)*T is (2t+1), so the buffer is
 *	filled up here. The buffer denoted by sharp(#) represents
 *	tr(F*T)*T truncated at t.
 *		ADDR: 0        (t-f+1)        t           2t 2t+1
 *		      V           V           V           V   V
 *		      +---+---+---+---+---+---+---+---+---+---+
 *		BUF   | X |  ...  | X |  ...  | X#|  ...  | X#|
 *		      +---+---+---+---+---+---+---+---+---+---+
 *
 *		Currently, we have approximate A(x_n)^2/C at the
 *	buffer with sharp(#) notation, so we negate this value.
 *	Here a problem arises. As the former evaluation of F*T^2
 *	implies, 2 * x_n - A(x_n)^2/C overestimates x_n, namely,
 *	sometimes we encounter slightly bigger B calculated. This
 *	makes conversion unstable.
 *
 *		Newton inverse converges if the series approaches
 *	the true value from below, since f(x) is convex.
 *		f(x) ^ |
 *		     | |
 *		     | |
 *		     |  |   f(x) = 2x - Ax^2/C
 *		     |   |
 *		     |    \
 *		     |     \-\
 *		     |        \--\
 *		     .------------\----\-------------> x
 *		     |                  \------\
 *		     |                          \--------
 *	But once the series overestimates and go beyond the true
 *	B, which satisfies AB = C, then the system may fall in
 *	an infinite loop that plays step dance of x_n around the
 *	actual B, which must be suppressed. For this reason, once
 *	we start to approach from below, the updates must not go
 *	beyond the actual B. If this property holds, the newton
 *	inverse will converge 'from below', and we aquire a near
 *	pin but slightly small B for AB = C.
 *
 *		Overestimation is always suppressed, if we adopt
 *	2 * x_n - (1 + A(x_n)^2/C) instead, since we have proved
 *		F*T^2 < (tr(F*T)*T + b^t)*b^(f-1)
 *	But this is also problematic. The approach may be smaller
 *	than near pin value. So this improvement doesn't solve
 *	the matter. Therefore a detection of overestimates have
 *	to be implemented. When overestimation is detected, that
 *	is the end of the algorithm.
 ****************************************************************
 */

int apm_base_array_inverse(
	apm_base_t *to,
	int to_length,
	apm_base_t *from,
	int from_length,
	apm_base_t *buf)
{
	int i;
	int total_length;
	apm_base_t carry;

	apm_base_array_zero(to, to_length - 1);
	to[to_length - 1] = APM_BASE_MAX
		/ from[from_length - 1];
//	total_length = 2 * (to_length + 1);
	total_length = 2 * to_length + 1;

//	for(i = 0; i < 100; ++i) {
	for(i = 0; i < 8; ++i) {
		printf("i = %d\n", i);

		printf("CAL0:\tfrom = ");
		print_number(from, from_length);
		printf("CAL0:\tto = ");
		print_number(to, to_length);

		apm_base_array_multiply(
			&buf[to_length - from_length + 1],
			to, to_length,
			from, from_length);
		printf("CAL1:\tbuf = ");
		print_number(buf, total_length);

		apm_base_array_multiply(
			&buf[0],
			&buf[to_length], to_length + 1,
			to, to_length);				// tricky
		printf("CAL3:\tbuf = ");
		print_number(buf, total_length);

#if 0
		apm_base_array_negate(
			&buf[to_length],
			&buf[to_length],
			to_length + 1);
#else
		apm_base_array_negative(
			&buf[to_length],
			to_length + 1,
			&buf[to_length],
			to_length + 1);
#endif
		printf("CAL5:\tbuf = ");
		print_number(buf, total_length);

		apm_base_array_add(
			&buf[to_length], &carry,
			to, to_length,
			&buf[to_length], to_length + 1,
			APM_BASE_ZERO);
		printf("CAL6:\tbuf = ");
		print_number(buf, total_length);

		apm_base_array_add(
			&buf[to_length], &carry,
			to, to_length,
			&buf[to_length], to_length + 1,
			APM_BASE_ZERO);
		printf("CAL7:\tbuf = ");
		print_number(buf, total_length);

		if(memcmp(to, &buf[to_length], to_length) == 0) {
			break;
		}
		apm_base_array_copy(
			to, &buf[to_length],
			to_length);
		printf("CAL9:\tto = ");
		print_number(to, to_length);
	}

	if(carry != APM_BASE_ZERO) {
		to[to_length] = carry;
		++to_length;
	}

//	printf("to_length = %d\n", to_length);
//	print_number(to, to_length);
}

/**
 ****************************************************************
 *
 ****************************************************************
 */

void add_test()
{
	unsigned int i, j;
	apm_base_t op1[sizeof(int)];
	apm_base_t op2[sizeof(int)];
	apm_base_t res[sizeof(int)];

	for(i = 0; i < 0x10000; ++i) {
		if(i % (0x10000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x10000/ 100));
		}
		for(j = 0; j < 0x10000; ++j) {
			apm_base_t carry;
			*(unsigned int *)op1 = i;
			*(unsigned int *)op2 = j;

			apm_base_array_add(
				res, &carry,
				op1, sizeof(int),
				op2, sizeof(int),
				APM_BASE_ZERO);

			if(*(unsigned int *)res != i + j) {
				printf("# ERROR i, j, res = %x, %x, %x\n",
					i, j, *(int *)res);
				return;
			}
		}
	}
}

void subtract_test()
{
	unsigned int i, j;
	apm_base_t op1[sizeof(int)];
	apm_base_t op2[sizeof(int)];
	apm_base_t neg[sizeof(int)];
	apm_base_t res[sizeof(int)];

	for(i = 0; i < 0x10000; ++i) {
		if(i % (0x10000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x10000/ 100));
		}
		for(j = 0; j < 0x10000; ++j) {
			apm_base_t carry;
			*(unsigned int *)op1 = i;
			*(unsigned int *)op2 = j;

			apm_base_array_negative(
				neg, sizeof(int),
				op2, sizeof(int));
			apm_base_array_add(
				res, &carry,
				op1, sizeof(int),
				neg, sizeof(int),
				APM_BASE_ZERO);

			if(*(unsigned int *)res != i - j) {
				printf("# ERROR i, j, res = %x, %x, %x\n",
					i, j, *(int *)res);
				return;
			}
		}
	}
}

void multiply_test()
{
	unsigned int i, j;
	apm_base_t op1[sizeof(int)];
	apm_base_t op2[sizeof(int)];
	apm_base_t res[sizeof(int) * 2];

	for(i = 0; i < 0x10000; ++i) {
		if(i % (0x10000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x10000/ 100));
		}
		for(j = 0; j < 0x10000; ++j) {
			apm_base_t carry;
			*(unsigned int *)op1 = i;
			*(unsigned int *)op2 = j;

			apm_base_array_multiply(
				res,
				op1, sizeof(int),
				op2, sizeof(int));

			if(*(unsigned int *)res != i * j) {
				printf("# ERROR i, j, res = %x, %x, %x\n",
					i, j, *(int *)res);
				return;
			}
		}
	}
}

int divide_try(int i, int j)
{
	apm_base_t op1[sizeof(int)];
	apm_base_t op2[sizeof(int)];
	apm_base_t inv[sizeof(int) + 2];
	apm_base_t buf[sizeof(int) * 2 + 1];
	apm_base_t res[sizeof(int) * 2 + 2];
	int len1, len2;
	apm_base_t carry;

	*(unsigned int *)op1 = i;
	*(unsigned int *)op2 = j;

	apm_base_array_resize(op1, sizeof(int), &len1);
	apm_base_array_resize(op2, sizeof(int), &len2);

	printf("i, j, len1, len2 = %d, %d, %d, %d\n",
		i, j, len1, len2);
	apm_base_array_inverse(
		inv, len1 + 1,
		op2, len2,
		buf);
	inv[len1 + 1] = APM_BASE_ZERO;
	printf("inv1:\t");
	print_number(inv, len1 + 2);
	apm_base_array_multiply(
		res,
		op2, len2,
		inv, len1 + 2);
	printf("one:\t");
	print_number(res, len1 + len2 + 1);
	apm_base_array_increment(
		//&inv[1], len1 + 1);
		inv, len1 + 2);
	printf("inv2:\t");
	print_number(inv, len1 + 2);
	apm_base_array_multiply(
		res,
		op2, len2,
		inv, len1 + 2);
	printf("one++:\t");
	print_number(res, len1 + len2 + 1);
	apm_base_array_multiply(
		res,
		op1, len1,
		inv, len1 + 2);
	printf("res:\t");
	print_number(res, 2 * len1 + 1);
	memcpy(res, &res[len1 + len2], len1 - len2 + 1);
	apm_base_array_zero(
		&res[len1 - len2 + 1],
		sizeof(int) - (len1 - len2 + 1)); // <- tekito

	if(*(unsigned int *)res != i / j) {
		printf("# ERROR i, j, res = %x, %x, %x\n",
			i, j, *(unsigned int *)res);
		return 1;
	}
	return 0;
}
void divide_test()
{
	unsigned int i, j;
	apm_base_t op1[sizeof(int)];
	apm_base_t op2[sizeof(int)];
	apm_base_t inv[sizeof(int) + 2];
	apm_base_t buf[sizeof(int) * 2 + 1];
	apm_base_t res[sizeof(int) * 2 + 1];

	//for(i = 0; i < 0x10000; ++i) {
	for(i = 0xff; i < 0x10000; ++i) {
	//for(i = 0x10; i < 0x10000; ++i) {
		if(i % (0x10000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x10000/ 100));
		}
		//for(j = 1; j < 0x10000; ++j) {
		for(j = 0xff; j < 0x10000; ++j) {
		//for(j = 0x11; j < 0x10000; ++j) {
			if(divide_try(i, j)) {
				return;
			}
		}
	}
}

int test()
{
#define INV_LEN 100
	apm_base_t op1[8];
	apm_base_t op2[8];
	apm_base_t inv[INV_LEN];
	apm_base_t res[INV_LEN * 2];
	apm_base_t buf[INV_LEN * 2 + 1];

//	((unsigned int *)op1)[0] = 1;
//	((unsigned int *)op1)[0] = 10000;
	((unsigned int *)op1)[0] = 0xa3bed297;
//	((unsigned int *)op1)[1] = 0x910ecc88;

	apm_base_array_inverse(
		inv, INV_LEN,
		op1, 4, //op1, 8,
		buf);
	apm_base_array_multiply(
		res,
		op1, 4, //op1, 8,
		inv, INV_LEN);
	printf("op1:\t");
	print_number(op1, 4);
	printf("op2:\t");
	print_number(op2, 4);
	printf("inv:\t");
	print_number(inv, INV_LEN);
	printf("res:\t");
	print_number(res, 4 + INV_LEN);
}

int main()
{
	int i;
	apm_base_t op1[sizeof(int)];
	apm_base_t op2[sizeof(int)];
	apm_base_t res[sizeof(int) * 2 + 1];


	//add_test();
	//subtract_test();
	//multiply_test();
	//divide_test();
	divide_try(0xff, 0xff);

	//test();

#if 0
	*(unsigned int *)op1 = 0xfffeffff;
	*(unsigned int *)op2 = 0xfffeffff;
	apm_base_array_multiply(
		res,
		op1, sizeof(int),
		op2, sizeof(int));
	print_number(res, 8);
#endif
}
