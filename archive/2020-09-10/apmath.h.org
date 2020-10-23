#ifndef _APMATH_H_
#define _APMATH_H_

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
	int length);
int apmath_base_array_resize(
	apmath_base_t *array,
	int max_length,
	int *length);
int apmath_base_array_compare(
	int *res,
	apmath_base_t *left,
	int left_length,
	apmath_base_t *right,
	int right_length);
int apmath_base_increment(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand);
int apmath_base_decrement(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand);

int apmath_base_add(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t operand1,
	apmath_base_t operand2,
	apmath_base_t carry_in);

/**
 ****************************************************************
 *
 ****************************************************************
 */

int apmath_base_array_zero(
	apmath_base_t *array,
	int length);
int apmath_base_array_max(
	apmath_base_t *array,
	int length);
int apmath_base_array_copy(
	apmath_base_t *to,
	apmath_base_t *from,
	int length);
int apmath_base_array_extend(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length);

/**
 ****************************************************************
 *
 ****************************************************************
 */
int apmath_base_array_negate(
	apmath_base_t *to,
	apmath_base_t *from,
	int length);

/**
 ****************************************************************
 *
 ****************************************************************
 */

int apmath_base_array_increment(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *operand,
	int operand_length);
int apmath_base_array_decrement(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *operand,
	int operand_length);
int apmath_base_array_add(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *short_operand,
	int short_operand_length,
	apmath_base_t *long_operand,
	int long_operand_length,
	apmath_base_t carry_in);
int apmath_base_array_negative(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length);
/**
 ****************************************************************
 *
 ****************************************************************
 */
int apmath_base_multiply(
	apmath_base_t *lower,
	apmath_base_t *upper,
	apmath_base_t operand1,
	apmath_base_t operand2);
int apmath_base_array_multiply(
	apmath_base_t *result,
	apmath_base_t *carry_out,
	apmath_base_t *short_operand,
	int short_operand_length,
	apmath_base_t *long_operand,
	int long_operand_length);
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
	apmath_base_t *buf);
int apmath_base_array_safe_inverse(
	apmath_base_t *to,
	int to_length,
	apmath_base_t *from,
	int from_length,
	apmath_base_t *buf);

/**
 ****************************************************************
 *
 ****************************************************************
 */

void add_test();
void subtract_test();
void multiply_test();
int inverse_try(unsigned int x);
void inverse_test();
int divide_try(
	unsigned int x,
	unsigned int y);
void divide_test();

#endif // _APMATH_H_
