#include <stdio.h>

/**
 ****************************************************************
 *
 ****************************************************************
 */
#define MASK 0x80

int variable_byte_encode(
	char *from,
	char *to,
	int len)
{
	int i;

#define BIT_LENGTH	(8 * sizeof(char))
// 1		: (n - 1) ~ 1
// 2		: (n - 2) ~ 2
// ...
// n - 1	: 1  ~ (n - 1)
	to[0] = 0;
	for(i = 0; i < len; ++i) {
		to[i] = (from[i] << i) | MASK;
		to[i + 1] |= from[i] >> ((BIT_LENGTH - 1) - i);
	}
#undef BIT_LENGTH
}


int shift_forward(
	char *from,
	char *to,
	int length,
	int shift,
	char *upper)
{
	int i;
	char tbyte;
	void *temp_from, *temp_to;

	tbyte = 0;
	for(i = 0; i < length; ++i) {
		to[i] |= tbyte;
		tbyte = from[i] >> (8 * sizeof(char) - shift);
		to[i] |= from[i] << shift;
	}
	if(upper != NULL) {
		*upper |= tbyte;
	}

	return 0;
}
int shift_backward(
	char *from,
	char *to,
	int length,
	int shift,
	char *lower)
{
	int i;
	char tbyte;
	void *temp_from, *temp_to;

	tbyte = 0;
	for(i = length - 1; i >= 0; --i) {
		to[i] |= tbyte;
		tbyte = from[i] << (8 * sizeof(char) - shift);
		to[i] |= from[i] >> shift;
	}
	if(lower != NULL) {
		*lower |= tbyte;
	}

	return 0;
}
#if 0
int encode(
	char *from,
	int len,
	char *to)
{
	int i, j;

	i = 0;
	for(i = 0; i < 7; ++i) {
		int shift;

		to[i] = (from[i] << shift) & ~MASK;
	}
	while(from[i] & MASK) {
		int shift;
		int n;

		shift = i % (8 - 1);
		
		++i;
	}
	num = data[0] & ~MASK;
}
#endif
// 0 - 6 : 0 - 1
// 7 - 13 : 1 - 2
// 14 - 20 : 2 - 3
// 21 - 27 : 3 - 4
// 28 - 34 : 4 - 5
// 35 - 41 : 5 - 6
// 42 - 48 : 6 - 7

int main()
{
	int buf, buf1, buf2;
	printf("hello, world\n");


	buf = 100;
	buf1 = buf2 = 0;
	shift_forward(
		(char *)&buf, (char *)&buf1,
		sizeof(int), 3, NULL);
	printf("from, to = %08x, %08x\n", buf, buf1);
	shift_backward(
		(char *)&buf1, (char *)&buf2,
		sizeof(int), 3, NULL);
	printf("from, to = %08x, %08x\n", buf1, buf2);
}
