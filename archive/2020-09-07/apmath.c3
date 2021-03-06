#include <stdio.h>
#include <string.h>

/**
 ****************************************************************
 *	Arbitrary Precision Mathematics Library
 ****************************************************************
 */

typedef unsigned char apmath_base_t;
typedef unsigned long apmath_super_t;

#define APMATH_BASE_ZERO		((apmath_base_t) 0)
#define APMATH_BASE_ONE			((apmath_base_t) 1)
#define APMATH_BASE_MAX			((apmath_base_t) -1)
#define APMATH_BASE_BIT_SIZE		(8 * sizeof(apmath_base_t))
#define APMATH_BASE_BIT_HALF_SIZE	(APMATH_BASE_BIT_SIZE / 2)
#define APMATH_BASE_FULL		(APMATH_BASE_MAX)
#define APMATH_BASE_HALF_FULL		(APMATH_BASE_FULL >> APMATH_BASE_BIT_HALF_SIZE)

void print_number(
	apmath_base_t *array,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		printf("%02x ", array[i]);
	}
	//printf("(%u)\n", *(unsigned short *)array);
	printf("(%u)", *(unsigned int *)array);
	printf("\n");
}
int apmath_base_array_resize(
	apmath_base_t *array,
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
int apmath_base_array_compare(
	int *res,
	apmath_base_t *left,
	int left_length,
	apmath_base_t *right,
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

int apmath_base_increment(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand)
{
	*result = operand + APMATH_BASE_ONE;

//	if(*result == APMATH_BASE_ZERO) {
	if(operand == APMATH_BASE_MAX) {
		*carry_out = APMATH_BASE_ONE;
	} else {
		*carry_out = APMATH_BASE_ZERO;
	}

	return 0;
}
int apmath_base_decrement(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand)
{
	*result = operand + APMATH_BASE_MAX;

//	if(*result == APMATH_BASE_MAX) {
	if(operand == APMATH_BASE_ZERO) {
		*carry_out = APMATH_BASE_MAX;
	} else {
		*carry_out = APMATH_BASE_ZERO;
	}

	return 0;
}

int apmath_base_add(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand1,
	apmath_base_t operand2,
	apmath_base_t carry_in)
{
	*result = operand1 + operand2 + carry_in;

	if(carry_in == APMATH_BASE_ZERO) {
		if(*result < operand1 || *result < operand2) {
			*carry_out = APMATH_BASE_ONE;
		} else {
			*carry_out = APMATH_BASE_ZERO;
		}
	} else {
		if(*result <= operand1 || *result <= operand2) {
			*carry_out = APMATH_BASE_ONE;
		} else {
			*carry_out = APMATH_BASE_ZERO;
		}
	}

	return 0;
}


/**
 ****************************************************************
 *
 ****************************************************************
 */

int apmath_base_array_zero(
	apmath_base_t *array,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		array[i] = APMATH_BASE_ZERO;
	}

	return 0;
}
int apmath_base_array_max(
	apmath_base_t *array,
	int length)
{
	int i;

	for(i = 0; i < length; ++i) {
		array[i] = APMATH_BASE_MAX;
	}

	return 0;
}
int apmath_base_array_copy(
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
int apmath_base_array_extend(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length)
{
	apmath_base_array_copy(
		to,
		from,
		from_length);
	apmath_base_array_zero(
		&to[from_length],
		to_length - from_length);
}

/**
 ****************************************************************
 *
 ****************************************************************
 */
int apmath_base_array_negate(
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

/**
 ****************************************************************
 *
 ****************************************************************
 */

int apmath_base_array_increment(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *operand,
	int operand_length)
{
	int i;
	apmath_base_t carry;

	carry = APMATH_BASE_ONE;
	for(i = 0; i < operand_length && carry != APMATH_BASE_ZERO; ++i) {
		apmath_base_increment(
			&result[i],
			&carry,
			operand[i]);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
int apmath_base_array_decrement(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *operand,
	int operand_length)
{
	int i;
	apmath_base_t carry;

	carry = APMATH_BASE_MAX;
	for(i = 0; i < operand_length && carry != APMATH_BASE_ZERO; ++i) {
		apmath_base_decrement(
			&result[i],
			&carry,
			operand[i]);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}

int apmath_base_array_add(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *short_operand,
	int short_operand_length,
	apmath_base_t *long_operand,
	int long_operand_length,
	apmath_base_t carry_in)
{
	int i;
	apmath_base_t carry;

	carry = carry_in;
	for(i = 0; i < short_operand_length; ++i) {
		apmath_base_add(
			&result[i],
			&carry,
			short_operand[i],
			long_operand[i],
			carry);
	}
	if(carry != APMATH_BASE_ZERO) {
		apmath_base_array_increment(
			&result[short_operand_length],
			NULL,
			&result[short_operand_length],
			long_operand_length - short_operand_length);
	}

	if(carry_out != NULL) {
		*carry_out = carry;
	}

	return 0;
}
int apmath_base_array_negative(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length)
{
#if 1
	apmath_base_array_extend(
		to,
		to_length,
		from,
		from_length);
#else
	apmath_base_array_zero(
		to,
		to_length);
	apmath_base_array_copy(
		to,
		from,
		from_length);
#endif
	apmath_base_array_negate(
		to,
		to,
		to_length);
	apmath_base_array_increment(
		to,
		NULL,
		to,
		to_length);
}
/**
 ****************************************************************
 *
 ****************************************************************
 */
int apmath_base_multiply(
	apmath_base_t *lower,
	apmath_base_t *upper,
	apmath_base_t operand1,
	apmath_base_t operand2)
{
	apmath_super_t temp;

	temp = (apmath_super_t)operand1 * (apmath_super_t)operand2;
	*lower = temp & APMATH_BASE_FULL;
	*upper = temp >> APMATH_BASE_BIT_SIZE;

	return 0;
	
}
int apmath_base_array_multiply(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *short_operand,
	int short_operand_length,
	apmath_base_t *long_operand,
	int long_operand_length)
{
	int i, j;
	int result_length;

	result_length = short_operand_length + long_operand_length;

	apmath_base_array_zero(
		result,
		long_operand_length);

	for(i = 0; i < short_operand_length; ++i) {
		apmath_base_t temp_carry;
		apmath_base_t mult[2];
		apmath_base_t carry[2];

		carry[0] = 0;
		carry[1] = 0;
		for(j = 0; j < long_operand_length; ++j) {
			apmath_base_multiply(
				&mult[0],
				&mult[1],
				short_operand[i],
				long_operand[j]);
			if(j >= long_operand_length - 1) {
#if 1
				if(i >= short_operand_length - 1) {
					apmath_base_add(
						&result[i + j],
						&temp_carry,
						result[i + j],
						mult[0],
						carry[0]);
					apmath_base_add(
						carry_out,
						&temp_carry,
						carry[1],
						mult[1],
						temp_carry);
					return 0;
				}
#endif
				result[i + j + 1] = carry[1];
			}
			apmath_base_array_add(
				&result[i + j],
				&temp_carry,
				&result[i + j],
				2,
				mult,
				2,
				carry[0]);
			carry[0] = carry[1];
			carry[1] = temp_carry;
		}
	}

	return 0;
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
 *	and updates of newton approximation is expressed
 *		x_(n+1) = x_n - f(x_n)/f'(x_n)
 *		        = x_n - (C/(x_n) - A)/(-C/(x_n)^2)
 *		        = x_n + x_n - A(x_n)^2/C
 *		        = 2 * x_n - A(x_n)^2/C
 *	We do this update with multi-precision natural number
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
 *		Newton inverse converges if the series approaches
 *	the true value from below, since our f(x) is convex.
 *		f(x) ^ |
 *		     | |
 *		     | |
 *		     |  |   f(x) = 2x - Ax^2/C
 *		     |   |
 *		     |    \         B
 *		     |     \-\      |
 *		     |        \--\  V
 *		     .------------\----\-------------> x
 *		     |                  \------\
 *		     |                          \--------
 *	But once the series overestimates and go beyond the true
 *	B, which satisfies AB = C, then the system may fall into
 *	an infinite loop that plays step dance of x_n around the
 *	actual B, which must be suppressed. For this reason, once
 *	we start to approach from below, the updates must not go
 *	beyond the actual B. If this property holds, the newton
 *	inverse will converge 'from below', and we aquire a near
 *	pin but slightly small B for AB = C.
 *
 *		Let F, T and b be 'from', 'to' and base
 *	respectively. Also f and t are byte lengths of F and T.
 *	Evaluations of the main calculation follow below.
 *		C = b^(f+t-1) = b^c
 *		b^(f-1) <= F < b^f
 *		b^(t-1) <= T < b^t
 *		X*b^x <= F*T < (X+1)*b^x <= C
 *		X*T*b^x <= F*T^2 < (X+1)*T*b^x
 *		X*T*b^(x-c) <= F*T^2/C < (X+1)*T*b^(x-c)
 *		2T-(X+1)*T*b^(x-c) < 2T-F*T^2/C <= 2T-X*T*b^(x-c)
 *		       ||                ||              ||
 *		      LEFT            FIXED PT.         RIGHT
 *	Here X*b^x is truncated approximation of F*T. If (x-c) is
 *	equal to or less than (-t), the +1 effect in (X+1) of
 *	LEFT becomes trivial. So set
 *		x = c-t = (f+t-1)-t = f-1
 *	Then truncating X*T at t yields good approximation of the
 *	minus part of the fixed point. In this condition LEFT and
 *	RIGHT satisfiels next inequality.
 *		RIGHT - LEFT < 1.0
 *	which is good.
 *
 *		Next dipicts convergence of the newton method.
 *	tr(x) is truncated x. ce(x) and fl(x) are a ceil and
 *	floor function, thus
 *		ce(LEFT)  = 2T - fl((X+1)*T*b^(x-c))
 *		          = 2T - tr((X+1)*T)
 *		ce(RIGHT) = 2T - fl(X*T*b(x-c))
 *		          = 2T - tr(X*T)
 *	There are cases which depend on the location of fixed
 *	point.
 *		                FIXED PT.
 *		             LEFT   V           RIGHT
 *		              <-----V-------------|
 *		--------+---------------+---------------+------->
 *		        @            ce(LEFT)       ce(RIGHT)
 *
 *		                        FIXED PT.
 *		             LEFT           V   RIGHT
 *		              <-------------V-----|
 *		--------+---------------+---------------+------->
 *		                    @=ce(LEFT)      ce(RIGHT)
 *
 *		                    FIXED PT.
 *		          LEFT        RIGHT
 *		           <------------V
 *		--------+---------------+---------------+------->
 *		        @       ce(LEFT)=ce(RIGHT)
 *	The at-mark(@) shows the solution in this algorithm. From
 *	the figure above, selecting ce(LEFT) not fl(RIGHT) is
 *	suitable for updating newton inverse.
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
 *		BUF   | X |  ...  | X |  ...  | X#|  ...# | X#|
 *		      +---+---+---+---+---+---+---+---+---+---+
 *
 *		Currently, we have approximate A(x_n)^2/C at the
 *	buffer with sharp(#) notation, so we negate this value.
 *	Here a problem arises. As the former evaluation of F*T^2
 *	implies, 2 * x_n - A(x_n)^2/C may overestimates x_n. We
 *	know
 *		tr(F*T)*T <= tr(F*T)*T + T < tr(F*T)*T + b^t
 *	and what we have got up until here as an approximate
 *	value of A(x_n)^2/C is the left most term, which is
 *	tr(F*T)*T, truncated at t, saved at buffer with sharp(#).
 *	Namely, sometimes we get slightly bigger B calculated
 *	along updates of newton methods, since current value of
 *	A(x_n)^2/C is the 'lower' bound. This makes system's
 *	conversion unstable as explained below.
 *
 *		Overestimation is always suppressed, if we adopt
 *	2 * x_n - (1 + A(x_n)^2/C) instead, since we have proved
 *		F*T^2 < (tr(F*T)*T + b^t)*b^(f-1)
 *	But this is also problematic. The fixed point in the
 *	updates may be smaller than the near pin value. So this
 *	improvement doesn't solve the whole matter. Therefore a
 *	detection of overestimates have to be implemented. When
 *	overestimation is detected, that means it is the end of
 *	the algorithm. The detection of overestimation fails
 *	if A is the min value A = 0x0100...00 and the newton
 *	inverse x_n to be calculated x_n = 0x00ff...ff. Since our
 *	x_n can hold just t bytes, and can afford to represent
 *	only up to 0x00ff...ff, and this does never overestimate
 *	for A = 0x0100...00.
 *
 *		To summerize the convergence, the next table
 *	explains the algorithm types.
 *		           [initial value]
 *		                  V
 *		+-------+---+---+---+---+-----+---+---+-----+---++---------------+
 *		|  x_n  | 0 |...|ini|...|[B]-1|[B]| B |[B]+1|...||      A*B      |
 *		+-------+---+---+---+---+-----+---+---+-----+---++---------------+
 *		|   |min| * |   |-->|-->|  @# |   X   |  X  | X || = 0x0100...00 |
 *		+   +---+---+---+---+---+-----+-------+-----+---++---------------+
 *		| A |2^n|   |   |-->|-->|  #  |   @   |  *  |   || = 0x0100...00 |
 *		+   +---+---+---+---+---+-----+---+---+-----+---++---------------+
 *		|   |o/w|   |   |-->|-->|     | @#|   |  *  |   || < 0x0100...00 |
 *		+-------+---+---+---+---+-----+---+---+-----+---++---------------+
 *		  @ : the newton inverse to be calculated
 *		  * : converge here if 2 * (x_n) - (A*(x_n)^2/C) for updates
 *		  # : converge here if 2 * (x_n) - (1 + A*(x_n)^2/C) for updates
 *		      this is right before the cell B.
 *		  X : not available due to byte length of 'to'
 *
 *		Casing is necessary for the convergence of
 *	star(*) algorithm, because the system may never stop by
 *	detecting overestimation when A is the min value. If min
 *	value casing is not adopted, checking whether the system
 *	has come to the fixed point is an alternative way, but it
 *	is wasteful since overestimation excludes most of the
 *	cases. Never resign overestimation, because then the
 *	system can go in a loop back and forth the actual fixed
 *	point.
 *
 * 		Another choice is the convergence type sharp(#),
 *	where the system converges in the case of the min A. Also
 *	sharp(#) type algorithm necessarily converges into the
 *	required newton inverse denoted by at-mark(@) except for
 *	A is 2^n, but it accordingly requires to detect whether A
 *	is 2^n, which is more costly than star(*) conversion.
 ****************************************************************
 */

int apmath_base_array_inverse(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length,
	apmath_base_t *buf)
{
	int i;
	int total_length;
	int from_below;
	apmath_base_t carry; // not used any more probably

/* 1.	If A = 'from' = 0x0100...000, then the newton inverse is
 *	0x00ff...ff. If so, when the newton method is converging
 *	from below, overestimation check after tr(F*T) does not
 *	work properly, because
 *		F*T = 0x0100...00 * 0x00ff...ff != 0x0100...00
 *	and the next that comes right after 0x00ff...ff is
 *	0x0100...000, when truncated yiels t bytes 0x00...00, wow
 *	zero, decreasing! We need to exclude this case.
 */
#if 0
	if(from[from_length - 1] == APMATH_BASE_ONE) {
		for(i = from_length - 1; i > 0; --i) {
			if(from[i - 1] != APMATH_BASE_ZERO) {
				goto out;
			}
		}
		apmath_base_array_max(to, to_length);
		return 0;
	}
out:
#endif

/* 2.	Initialization of x_n. x_0 should be set to be strictly
 *	less than the actual inverse of the input. This is a
 *	conservative method.
 *	Let x be the most significant byte in 'from', which is
 *	from[f-1]. We are going to use only this information to
 *	set a candidate. Evaluation of C/A is
 *		C = b^(f+t-1)
 *		x*b^(f-1) <= A = 'from' <  (x+1)*b^(f-1)
 *		b^(-f+1)/(x+1) < 1/A <= b^(-f+1)/x
 *		b^(t)/(x+1) < C/A <= b^(t)/x
 *	Because we must choose a strictly smaller candidate than
 *	C/A, b^t/(x+1) is the best one. But in reality,
 *	((b-1)/(x+1))b^(t-1) is easy for computation, and this
 *	becomes 0 if x = b-1, so requires casing.
 */
	apmath_base_array_zero(to, to_length);
	//apmath_base_array_zero(to, to_length);
#if 1
	if(from[from_length - 1] < APMATH_BASE_MAX) {
		to[to_length - 1] = APMATH_BASE_MAX
			/ (from[from_length - 1] + 1);
	} else {
		to[to_length - 1] = APMATH_BASE_ONE;
	}
#else
	/* this is super conservative */
	to[to_length - 1] = APMATH_BASE_ONE;
#endif

	//total_length = 2 * to_length + 1;
	total_length = 2 * to_length;

/* 3.	Calculation of newton inverse. Loop executes, F*T
 *	multiplication, a detection of overestimation, F*T^2
 *	multiplication, necessary truncation, negation of F*T^2,
 *	and add T twice, these in this order.
 */
	for(i = 0; i < 256; ++i) {
//	for(i = 0; i < 16; ++i) {
		apmath_base_t msbyte;

#ifdef DEBUG
		printf("i = %d\n", i);
		printf("CAL0:\tfrom = ");
		print_number(from, from_length);
		printf("CAL0:\tto = ");
		print_number(to, to_length);
#endif

		apmath_base_array_multiply(
			&buf[to_length - from_length + 1],
			&msbyte,
			to, to_length,
			from, from_length);
#ifdef DEBUG
		printf("CAL1:\tbuf = ");
		print_number(buf, total_length);
		//printf("msbyte = %x\n", msbyte);
#endif

		if(msbyte > 0) {
			apmath_base_array_decrement(
				to, NULL, to, to_length);
			break;
		}
#if 0
		apmath_base_array_increment(
			&buf[to_length], NULL,
			&buf[to_length], to_length);
#endif

		apmath_base_array_multiply(
			&buf[0], &buf[2 * to_length - 1],
			&buf[to_length], to_length,
			to, to_length);
#ifdef DEBUG
		printf("CAL3:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_add(
			&buf[0], &carry,
			to, to_length,
			&buf[0], 2 * to_length,
			APMATH_BASE_ZERO);
#ifdef DEBUG
		printf("CAL4:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_negative(
			&buf[to_length],
			to_length,
			&buf[to_length],
			to_length);
#ifdef DEBUG
		printf("CAL5:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_add(
			&buf[to_length], &carry,
			to, to_length,
			&buf[to_length], to_length,
			APMATH_BASE_ZERO);
#ifdef DEBUG
		printf("CAL6:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_add(
			&buf[to_length], &carry,
			to, to_length,
			&buf[to_length], to_length,
			APMATH_BASE_ZERO);
#ifdef DEBUG
		printf("CAL7:\tbuf = ");
		print_number(buf, total_length);
#endif

		if(memcmp(to, &buf[to_length], to_length) == 0) {
			break;
		}

		apmath_base_array_copy(
			to, &buf[to_length],
			to_length);
#ifdef DEBUG
		printf("CAL9:\tto = ");
		print_number(to, to_length);
#endif
	}

	if(i >= 16) {
		printf("over: ");
		print_number(from, from_length);
	}

#ifdef DEBUG
	printf("LAST:\tto = ");
	print_number(to, to_length);
#endif
}

/**
 ****************************************************************
 *
 ****************************************************************
 */

void add_test()
{
	unsigned int i, j;
	apmath_base_t op1[sizeof(int)];
	apmath_base_t op2[sizeof(int)];
	apmath_base_t res[sizeof(int)];

	for(i = 0; i < 0x10000; ++i) {
		if(i % (0x10000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x10000/ 100));
		}
		for(j = 0; j < 0x10000; ++j) {
			apmath_base_t carry;
			*(unsigned int *)op1 = i;
			*(unsigned int *)op2 = j;

			apmath_base_array_add(
				res, &carry,
				op1, sizeof(int),
				op2, sizeof(int),
				APMATH_BASE_ZERO);

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
	apmath_base_t op1[sizeof(int)];
	apmath_base_t op2[sizeof(int)];
	apmath_base_t neg[sizeof(int)];
	apmath_base_t res[sizeof(int)];

	for(i = 0; i < 0x10000; ++i) {
		if(i % (0x10000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x10000/ 100));
		}
		for(j = 0; j < 0x10000; ++j) {
			apmath_base_t carry;
			*(unsigned int *)op1 = i;
			*(unsigned int *)op2 = j;

			apmath_base_array_negative(
				neg, sizeof(int),
				op2, sizeof(int));
			apmath_base_array_add(
				res, &carry,
				op1, sizeof(int),
				neg, sizeof(int),
				APMATH_BASE_ZERO);

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
	apmath_base_t op1[sizeof(int)];
	apmath_base_t op2[sizeof(int)];
	apmath_base_t res[sizeof(int) * 2];

	for(i = 0; i < 0x10000; ++i) {
		if(i % (0x10000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x10000/ 100));
		}
		for(j = 0; j < 0x10000; ++j) {
			apmath_base_t carry;
			*(unsigned int *)op1 = i;
			*(unsigned int *)op2 = j;

			apmath_base_array_multiply(
				res,
				&res[2 * sizeof(int) - 1],
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

int divide_try(
	unsigned int x,
	unsigned int y)
{
	apmath_base_t op1[sizeof(int)];
	apmath_base_t op2[sizeof(int)];
	apmath_base_t inv[sizeof(int) + 2];
	apmath_base_t buf[sizeof(int) * 2 + 1];
	apmath_base_t res[sizeof(int) * 2 + 2];
	int len1, len2;
	apmath_base_t carry;

	*(unsigned int *)op1 = x;
	*(unsigned int *)op2 = y;

	apmath_base_array_resize(op1, sizeof(int), &len1);
	apmath_base_array_resize(op2, sizeof(int), &len2);

	apmath_base_array_inverse(
		inv, len1 + 1,
		op2, len2,
		buf);
	inv[len1 + 1] = APMATH_BASE_ZERO;
#ifdef DEBUG
	printf("inv1:\t");
	print_number(inv, len1 + 2);
#endif
	apmath_base_array_multiply(
		res,
		&res[len1 + len2 + 1],
		op2, len2,
		inv, len1 + 2);
#ifdef DEBUG
	printf("one:\t");
	print_number(res, len1 + len2 + 2);
#endif

#if 0
	{	int i;
		for(i = 0; i < len1; ++i) {
			if(res[len2 + i] != APMATH_BASE_MAX) {
				printf("INVERSE ERROR");
				return 1;
			}
		}
	}
#endif
	apmath_base_array_increment(
		inv, NULL, inv, len1 + 2);
#ifdef DEBUG
	printf("inv2:\t");
	print_number(inv, len1 + 2);
#endif
	apmath_base_array_multiply(
		res,
		&res[len1 + len2 + 1],
		op2, len2,
		inv, len1 + 2);
#ifdef DEBUG
	printf("one++:\t");
	print_number(res, len1 + len2 + 2);
#endif
	apmath_base_array_multiply(
		res,
		&res[len1 + len1 + 1],
		op1, len1,
		inv, len1 + 2);
#ifdef DEBUG
	printf("op1:\t");
	print_number(op1, len1);
	printf("op2:\t");
	print_number(op2, len2);

	printf("res:\t");
	print_number(res, 2 * len1 + 2);
#endif
	memcpy(res, &res[len1 + len2], len1 - len2 + 1);
	apmath_base_array_zero(
		&res[len1 - len2 + 1],
		sizeof(int) - (len1 - len2 + 1)); // <- tekito

	if(*(unsigned int *)res != x / y) {
		printf("# ERROR x, y, res = %x, %x, %x\n",
			x, y, *(unsigned int *)res);
		return 1;
	}
	return 0;
}
void divide_test()
{
	unsigned int i, j;

	for(i = 0; i < 0x10000; ++i) {
		if(i % (0x10000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x10000/ 100));
		}
		for(j = 1; j < 0x10000; ++j) {
			if(divide_try(i, j)) {
				return;
			}
		}
	}
}

int test()
{
//#define INV_LEN 8
//#define INV_LEN 100
#define INV_LEN 10000
	apmath_base_t op1[8];
	apmath_base_t op2[8];
	apmath_base_t inv[INV_LEN];
	apmath_base_t res[INV_LEN * 2];
	apmath_base_t buf[INV_LEN * 2 + 1];

//	((unsigned int *)op1)[0] = 1;
//	((unsigned int *)op1)[0] = 10000;
	((unsigned int *)op1)[0] = 0xa3bed297;
//	((unsigned int *)op1)[1] = 0x910ecc88;

	apmath_base_array_inverse(
		inv, INV_LEN,
		op1, 4, //op1, 8,
		buf);
	apmath_base_array_multiply(
		res,
		&res[4 + INV_LEN - 1],
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
	apmath_base_t op1[sizeof(int)];
	apmath_base_t op2[sizeof(int)];
	apmath_base_t res[sizeof(int) * 2 + 1];


	//add_test();
	//subtract_test();
	//multiply_test();
	divide_test();

	//divide_try(0x0100, 0x0001);
	//divide_try(0x01, 0x01);
	//divide_try(0xff, 0xff);
	//divide_try(0x102, 0x103);
	//divide_try(0x1f4, 0x1f4);
	//divide_try(97, 4097);
	//divide_try(97, 35925);
	//divide_try(97, 29382);

	//test();

#if 0
	*(unsigned int *)op1 = 0xfffeffff;
	*(unsigned int *)op2 = 0xfffeffff;
	apmath_base_array_multiply(
		res,
		op1, sizeof(int),
		op2, sizeof(int));
	print_number(res, 8);
#endif
}
