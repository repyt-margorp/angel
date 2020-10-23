#include "apmath.h"

/**
 ****************************************************************
 *	Arbitrary Precision Mathematics Library
 ****************************************************************
 */

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
	*upper = temp >> APMATH_BASE_BIT_LENGTH;

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
 *		--------@---------------+---------------+------->
 *		                     ce(LEFT)       ce(RIGHT)
 *		     fl(LEFT)       fl(RIGHT)
 *
 *		                        FIXED PT.
 *		             LEFT           V   RIGHT
 *		              <-------------V-----|
 *		--------+---------------@---------------+------->
 *		                     ce(LEFT)       ce(RIGHT)
 *		     fl(LEFT)       fl(RIGHT)
 *
 *		                    FIXED PT.
 *		          LEFT        RIGHT
 *		           <------------V
 *		--------@---------------+---------------+------->
 *		                ce(LEFT)=ce(RIGHT)
 *		     fl(LEFT)       fl(RIGHT)
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
 *		ADDR: 0        (t-f+1)        t           2t  msbyte
 *		      V           V           V           V     V
 *		      +---+---+---+---+---+---+---+---+---+   +---+
 *		BUF   |   |  ...  | X |  ...  | X*|  ...* |   | X*|
 *		      +---+---+---+---+---+---+---+---+---+   +---+
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
 *		ADDR: 0        (t-f+1)        t           2t  msbyte
 *		      V           V           V           V     V
 *		      +---+---+---+---+---+---+---+---+---+   +---+
 *		BUF   | X |  ...  | X |  ...  | X#|  ...# |   | X#|
 *		      +---+---+---+---+---+---+---+---+---+   +---+
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
	unsigned int to_precision;
	unsigned int from_precision;
	unsigned int precision;
	int gonna_precise;
	int total_length;
	apmath_base_t carry; // not used any more probably

	total_length = 2 * to_length;

/* 1.	Initialization of x_n. x_0 should be set to be strictly
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
//	apmath_base_array_zero(to, to_length);
#if 1
	if(from[from_length - 1] < APMATH_BASE_MAX) {
		//apmath_base_array_zero(to, to_length);
		to[to_length - 1] = APMATH_BASE_MAX
			/ (from[from_length - 1] + 1);
	} else {
		//apmath_base_array_zero(to, to_length);
		to[to_length - 1] = APMATH_BASE_ONE;
	}
#else
	/* this is super conservative */
	apmath_base_array_zero(to, to_length);
	//apmath_base_array_zero(to, to_length - 1);
	to[to_length - 1] = APMATH_BASE_ONE;
#endif

#if 1
	to_precision = 1;
	from_precision = 1;
	if(to_length >= 2) {
		//precision = 2;
		precision = 1;
	} else {
		precision = 1;
	}
	gonna_precise = 0;
#else
	/* this does not follow precision */
	to_precision = to_length;
	from_precision = from_length;
	precision = to_length;
	gonna_precise = 1;
#endif
	while(to_precision < to_length) {
		apmath_base_t msbyte;

		if(gonna_precise) {
			precision = to_length;
			to_precision = to_length;
			from_precision = from_length;
		} else {
			to_precision = precision;
			precision *= 2;
			if(precision == 0) {
				/* if hyper big array */
				precision = -1;
			}
			if(precision >= to_length) {
				gonna_precise = 1;
				precision = to_length;
			}
			if(precision > from_length) {
				from_precision = from_length;
			} else {
				from_precision = precision;
			}
			//from_precision = from_length;
		}
#ifdef DEBUG
		printf("to_precision = %x\n", to_precision);
		printf("from_precision = %x\n", from_precision);
		printf("precision = %x\n", precision);
		printf("gonna_precise = %d\n", gonna_precise);
		printf("PRE0:\tfrom = ");
		print_number(from, from_length);
		printf("PRE0:\tto = ");
		print_number(to, to_length);
		apmath_base_array_zero(buf, 2*to_length);
#endif

		apmath_base_array_multiply(
			&buf[2 * to_length - (to_precision + from_precision - 1)],
			&msbyte,
			&to[to_length - to_precision],
			to_precision,
			&from[from_length - from_precision],
			from_precision);
#ifdef DEBUG
		printf("msbyte = %d\n", msbyte);
		printf("PRE1:\tbuf = ");
		print_number(buf, total_length);
#endif

		if(from_precision < from_length) {
			apmath_base_array_add(
				&buf[2 * to_length - (to_precision + from_precision - 1)],
				&carry,
				&to[to_length - to_precision],
				to_precision,
				&buf[2 * to_length - (to_precision + from_precision - 1)],
				to_precision + from_precision - 1,
				APMATH_BASE_ZERO);
		} else {
			apmath_base_array_increment(
				&buf[(2 * to_length) - to_precision],
				&carry,
				&buf[(2 * to_length) - to_precision],
				to_precision);
		}
#ifdef DEBUG
		printf("PRE2:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_multiply(
			&buf[2 * to_length - 2 * to_precision],
			&buf[2 * to_length - 1],
			&buf[(2 * to_length) - to_precision],
			to_precision,
			&to[to_length - to_precision],
			to_precision);
#ifdef DEBUG
		printf("PRE3:\tbuf = ");
		print_number(buf, total_length);
#endif

		if(msbyte > 0 || carry > 0) {
			apmath_base_array_add(
				&buf[(2 * to_length) - to_precision],
				&carry,
				&to[to_length - to_precision],
				to_precision,
				&buf[(2 * to_length) - to_precision],
				to_precision,
				APMATH_BASE_ZERO);
		}
#ifdef DEBUG
		printf("PRE4:\tbuf = ");
		print_number(buf, total_length);
#endif
/*
		if(msbyte > 0) {
			apmath_base_array_add(
				&buf[(2 * to_length) - to_precision],
				&carry,
				&to[to_length - to_precision],
				to_precision,
				&buf[(2 * to_length) - to_precision],
				to_precision,
				APMATH_BASE_ZERO);
		}
*/

		apmath_base_array_negate(
			&buf[(2 * to_length) - precision],
			precision,
			&buf[(2 * to_length) - precision],
			precision);
#ifdef DEBUG
		printf("PRE5:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_add(
			&buf[(2 * to_length) - to_precision],
			&carry,
			&buf[(2 * to_length) - to_precision],
			to_precision,
			&to[to_length - to_precision],
			to_precision,
			APMATH_BASE_ZERO);
#ifdef DEBUG
		printf("PRE6:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_add(
			&buf[(2 * to_length) - to_precision],
			&carry,
			&buf[(2 * to_length) - to_precision],
			to_precision,
			&to[to_length - to_precision],
			to_precision,
			APMATH_BASE_ZERO);
#ifdef DEBUG
		printf("PRE7:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_copy(
			&to[to_length - precision],
			&buf[(2 * to_length) - precision],
			precision);
#ifdef DEBUG
		printf("PRE9:\tto = ");
		print_number(to, to_length);
#endif
	}


/* 2.	Calculation of newton inverse. Loop executes, F*T
 *	multiplication, a detection of overestimation, F*T^2
 *	multiplication, necessary truncation, negation of F*T^2,
 *	and add T twice, these in this order.
 */
#ifdef DEBUG
	printf("to_precision = %x\n", to_precision);
	printf("from_precision = %x\n", from_precision);
	printf("precision = %x\n", precision);
	printf("gonna_precise = %d\n", gonna_precise);
	printf("PRECAL:\tto = ");
	print_number(to, to_length);
#endif

//	for(i = 0; i < 256; ++i) {
//	for(i = 0; i < 32; ++i) {
	for(i = 0; i < 16; ++i) {
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
			&to[0],
			to_length,
			&from[0],
			from_length);
#ifdef DEBUG
		printf("CAL1:\tbuf = ");
		print_number(buf, total_length);
		printf("msbyte = %x\n", msbyte);
#endif

		if(msbyte > 0) {
			apmath_base_array_decrement(
				&to[0],
				NULL,
				&to[0],
				to_length);
			break;
		}
		apmath_base_array_multiply(
			&buf[0],
			&buf[2 * to_length - 1],
			&buf[to_length],
			to_length,
			&to[0],
			to_length);
#ifdef DEBUG
		printf("CAL3:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_add(
			&buf[0],
			&carry,
			&to[0],
			to_length,
			&buf[0],
			2 * to_length,
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
			&buf[to_length],
			&carry,
			&to[0],
			to_length,
			&buf[to_length],
			to_length,
			APMATH_BASE_ZERO);
#ifdef DEBUG
		printf("CAL6:\tbuf = ");
		print_number(buf, total_length);
#endif

		apmath_base_array_add(
			&buf[to_length],
			&carry,
			&to[0],
			to_length,
			&buf[to_length],
			to_length,
			APMATH_BASE_ZERO);
#ifdef DEBUG
		printf("CAL7:\tbuf = ");
		print_number(buf, total_length);
#endif

		if(memcmp(to, &buf[to_length], to_length) == 0) {
			break;
		}

		apmath_base_array_copy(
			&to[0],
			&buf[to_length],
			to_length);
#ifdef DEBUG
		printf("CAL9:\tto = ");
		print_number(to, to_length);
#endif
	}

	if(i >= 16) {
		printf("over: i=%d; ", i);
		print_number(from, from_length);
	}

#ifdef DEBUG
	printf("LAST:\tto = ");
	print_number(to, to_length);
#endif
}
int apmath_base_array_safe_inverse(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length,
	apmath_base_t *buf)
{
	int i;
	apmath_base_t carry;

	apmath_base_array_inverse(
		to, to_length,
		from, from_length,
		buf);

	apmath_base_array_multiply(
		&buf[0],
		&buf[from_length + to_length - 1],
		to, to_length,
		from, from_length);
	for(i = 0; i < to_length; ++i) {
		if(i < to_length - 1) {
			if(buf[from_length + i] == APMATH_BASE_MAX) {
				continue;
			}
		} else {
			if(buf[from_length + i] == APMATH_BASE_ZERO) {
				continue;
			}
		}
		printf("# inverse error:\n");
		printf("from(len = %d):\n", from_length);
		print_number(from, from_length);
		printf("to(len = %d):\n", to_length);
		print_number(to, to_length);
		printf("one:\n");
		print_number(buf, from_length + to_length);
		return 1;
	}

	apmath_base_array_add(
		&buf[0], &carry,
		from, from_length,
		&buf[0], from_length + to_length,
		APMATH_BASE_ZERO);
	for(i = 0; i < to_length; ++i) {
		if(i < to_length - 1) {
			if(buf[from_length + i] == APMATH_BASE_ZERO) {
				continue;
			}
		} else {
			if(buf[from_length + i] == APMATH_BASE_ONE) {
				continue;
			}
		}
		printf("# inverse error:\n");
		printf("from(len = %d):\n", from_length);
		print_number(from, from_length);
		printf("to(len = %d):\n", to_length);
		print_number(to, to_length);
		printf("one++:\n");
		print_number(buf, from_length + to_length);
		return 1;
	}

	return 0;
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

int inverse_try(unsigned int x)
{
#define INVERSE_LENGTH 100
	unsigned int i;
	apmath_base_t op[sizeof(int)];
	apmath_base_t inv[INVERSE_LENGTH];
	apmath_base_t buf[INVERSE_LENGTH * 2];
	int prec[] = { 8, 16, 32, 64 };
	int len;

	*(unsigned int *)op = x;
	apmath_base_array_resize(&len, op, sizeof(int));
	prec[0] = len;
	prec[1] = len + 1;
	for(i = 0; i < sizeof(prec) / sizeof(int); ++i) {
		if(apmath_base_array_safe_inverse(
			inv, prec[i],
			op, len,
			buf))
		{
			return 1;
		}
	}

	return 0;
}
void inverse_test()
{
	unsigned int i, j;

	for(i = 0; i < 0x1000000; ++i) {

		if(i % (0x1000000 / 100) == 0) {
			printf("%d/100 finished\n",
				i / (0x1000000 / 100));
		}
		if(inverse_try(i + 1)) {
			return;
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

	apmath_base_array_resize(&len1, op1, sizeof(int));
	apmath_base_array_resize(&len2, op2, sizeof(int));

	apmath_base_array_safe_inverse(
	//apmath_base_array_inverse(
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
	apmath_base_t buf[INV_LEN * 2];

//	((unsigned int *)op1)[0] = 1;
//	((unsigned int *)op1)[0] = 10000;
	((unsigned int *)op1)[0] = 0xa3bed297;
//	((unsigned int *)op1)[1] = 0x910ecc88;

	//apmath_base_array_inverse(
	apmath_base_array_safe_inverse(
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


#if 1
int main()
{
	int i;

	//add_test();
	subtract_test();
	//multiply_test();
	//inverse_test();
	//divide_test();

	//inverse_try(0x10);
	//inverse_try(0x80);
	//inverse_try(0xff);
	//inverse_try(0x02c0);
	//inverse_try(0xffff00);
	//inverse_try(0x02ddc6);

	//divide_try(0x0100, 0x0001);
	//divide_try(0x01, 0x01);
	//divide_try(0xff, 0xff);

	//test();
}
#endif
