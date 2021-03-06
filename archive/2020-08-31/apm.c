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

#if 0
	apm_base_array_zero(
		result,
		short_operand_length);

	for(i = 0; i < result_length; ++i) {
		int from, to;

		if(short_operand_length + i < result_length) {
			result[short_operand_length + i] = 0;
		}
		if(i < short_operand_length) {
			from = 0;
			to = i + 1;
		} else if(i < long_operand_length) {
			from = 0;
			to = short_operand_length;
		} else {
			from = i - long_operand_length;
			to = short_operand_length;
		}
		for(j = from; j < to; ++j) {
			apm_base_t carry;	// go to trush
			apm_base_t temp[2];

			apm_base_multiply(
				&temp[0],
				&temp[1],
				short_operand[j],
				long_operand[i - j]);
			apm_base_array_add(
				&result[i],
				&carry,
				temp,
				2,
				&result[i],
				result_length - i,
				APM_BASE_ZERO);
		}
	}
#else
//	apm_base_array_zero(
//		result,
//		result_length);

	apm_base_array_zero(
		result,
		long_operand_length);
	for(i = 0; i < short_operand_length; ++i) {
		for(j = 0; j < long_operand_length; ++j) {
			apm_base_t carry;	// go to trush
			apm_base_t temp[2];

			apm_base_multiply(
				&temp[0],
				&temp[1],
				short_operand[i],
				long_operand[j]);
			if(j == long_operand_length - 1 &&
				long_operand_length + i < result_length)
			{
				result[long_operand_length + i] = 0;
			}
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
#endif
}
/**
 ****************************************************************
 *	Let F, T and b be 'from', 'to' and base respectively.
 *	Also tr(x) is an appropriate byte trucation of x, so
 *	tr(x) excludes number of least significant bytes and
 *	here tr(x)*b^n is nearly x. Using these notations, the
 *	evaluation of F*T truncated at (f-1) is
 *		tr(F*T)*b^(f-1) <= F*T < (tr(F*T) + 1)*b^(f-1)
 *	where f and t are byte lengths of F and T. Then multiply
 *	every term in the inequality above by T. This yields
 *	F*T^2 evaluation.
 *		tr(F*T)*T*b^(f-1) <= F*T^2 < (tr(F*T) + 1)*T*b^(f-1)
 *		                           = (tr(F*T)*T + T)*b^(f-1)
 *	Since F times T yields (f+t) bytes results, tr(F*T) is
 *	(t+1) (= (f+t) - (f-1)) bytes. Accordingly tr(F*T)*T is
 *	(2t+1) bytes. We are not interested in the lower (f+t-1)
 *	bytes of F*T^2. Thus from the above evaluation on F*T^2,
 *	another t (= (f+t-1) - (f-1)) bytes are to be ignored
 *	from upper bytes of F*T^2, which is tr(F*T)*T or
 *	(tr(F*T)*T + T). How different are tr(F*T)*T and
 *	(tr(F*T)*T + T)? Recall that T is less than b^t
 *	(T < b^t), so
 *		tr(F*T)*T <= tr(F*T)*T + T < tr(F*T)*T + b^t
 *	Therefore, the whole evaluation of F*T^2 is bound above.
 *		F*T^2 < (tr(F*T)*T + b^t)*b^(f-1)
 *	To calculate upper bytes of F*T^2, we only need to
 *	truncate tr(F*T)*T and (tr(F*T)*T + b^t) at t. But this
 *	calculation is applying tr again for tr(F*T)*T, namely
 *	tr(tr(F*T)*), and decide where to add 1 or not. In other
 *	words, F*T^2, which is (f+2t) bytes, truncated at (f+t-1)
 *	yields (t+1) bytes result tr(tr(F*T)*T).
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
	int temp_length;
	int total_length;
	apm_base_t carry;

	temp_length = 1;
	to[to_length - 1] = APM_BASE_MAX
		/ from[from_length - 1];
	//*to = APM_BASE_ONE;
// min of the next two value is correct program.
	//total_length = from_length + to_length;
	total_length = to_length + to_length + 1;
//	for(i = 0; ; i = 0) {
	for(i = 0; i < 100; ++i) {
		printf("i = %d\n", i);

//		printf("CAL0:\tfrom = ");
//		print_number(from, from_length);
//		printf("CAL0:\tto = ");
//		print_number(to, to_length);

		apm_base_array_multiply(
			buf,
			to, to_length,
			from, from_length);
//		printf("CAL1:\tbuf = ");
//		print_number(buf, total_length);

		memmove(
			&buf[to_length],
			&buf[from_length - 1],
			//to_length);
			to_length + 1);
//		printf("CAL2:\tbuf = ");
//		print_number(buf, total_length);

		apm_base_array_multiply(
			&buf[0],
			&buf[to_length], to_length + 1,	// tricky
			to, to_length);			// tricky
//		printf("CAL3:\tbuf = ");
//		print_number(buf, total_length);

#if 0
		apm_base_array_increment(
			&buf[to_length],
			to_length);
#endif
//		printf("CAL8:\tbuf = ");
//		print_number(buf, total_length);

#if 0
		apm_base_array_negative(
			&buf[to_length], to_length + 1,
			&buf[to_length], to_length + 1);
#else
		apm_base_array_negate(
			&buf[to_length],
			&buf[to_length],
			to_length + 1);
#endif
//		printf("CAL5:\tbuf = ");
//		print_number(buf, total_length);

		apm_base_array_add(
			&buf[to_length], &carry,
			to, to_length,
			&buf[to_length], to_length + 1,
			APM_BASE_ZERO);
//		printf("CAL6:\tbuf = ");
//		print_number(buf, total_length);

		apm_base_array_add(
			&buf[to_length], &carry,
			to, to_length,
			&buf[to_length], to_length + 1,
			APM_BASE_ZERO);
//		printf("CAL7:\tbuf = ");
//		print_number(buf, total_length);

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

	printf("to_length = %d\n", to_length);
	print_number(to, to_length);
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

			if(*(int *)res != i + j) {
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

			if(*(int *)res != i - j) {
				printf("# ERROR i, j, neg, res = %x, %x, %x, %x\n",
					i, j, *(int *)neg, *(int *)res);
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

			if(*(int *)res != i * j) {
				printf("# ERROR i, j, res = %x, %x, %x\n",
					i, j, *(int *)res);
				return;
			}
		}
	}
}

int test()
{
	apm_base_t op1[8];
	apm_base_t op2[8];
	apm_base_t inv[32];
	apm_base_t res[100];
	apm_base_t buf[100];

//	((unsigned int *)op1)[0] = 10000;
	((unsigned int *)op1)[0] = 0xa3bed297;
	((unsigned int *)op1)[1] = 0x910ecc88;

#define INV_LEN
	apm_base_array_inverse(
		inv, INV_LEN,
		op1, 8,
		buf);
	apm_base_array_multiply(
		res,
		op1, 8,
		inv, INV_LEN);
	printf("op1:\t");
	print_number(op1, 8);
	printf("op2:\t");
	print_number(op2, 8);
	printf("inv:\t");
	print_number(inv, INV_LEN);
	printf("res:\t");
	print_number(res, 8 * INV_LEN);
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

	test();
#if 0
	//for(i = 2; i < 256; ++i) {
	for(i = 256; i < 257; ++i) {
		int j;
		unsigned long long x;
		int len;
		int div;
		apm_base_t inv[sizeof(int)];
		apm_base_t num[sizeof(int)];
		apm_base_t buf[sizeof(int) * 2 + 1];

		*(unsigned int *)num = i;
		for(len = sizeof(int); len > 0; --len) {
			if(num[len - 1] != 0) {
				break;
			}
		}
		apm_base_array_zero(inv, sizeof(int));
		apm_base_array_inverse(
			inv, 2,
			num, len, // num, 2,
			buf);

		x = 1;
		div = 256;
		for(j = 0; j < len; ++j) {
			div *= 256;
		}
		for(j = 0; j < 25; ++j) {
			unsigned long long up;
//			up = 2 * x - i * x * x / 65536;
			up = 2 * x - i * x * x / div;
			printf("update of x[%2d]:\t%d -> %d\n", j, x, up);
			if(x == up) {
				break;
			}
			x = up;
		}
//		printf("x = %d\n", x);
//		print_number(inv, 2);
		if(*(unsigned int *)inv != x) {
			printf("# ERROR i, x, inv = %x, %x, %x\n",
				i, x, *(unsigned int *)inv);
			return 0;
		}
	}
#endif

#if 0
	*(unsigned int *)op1 = 1;
	*(unsigned int *)op2 = 1;
	apm_base_array_multiply(
		res,
		op1, sizeof(int),
		op2, sizeof(int));
	print_number(res, sizeof(int));
#endif
}
