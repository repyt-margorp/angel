#include <stdio.h>

int main()
{
	int i;
	int x;

#define MAX	65536
#define NUM	70	
	x = 1;
	for(i = 0; i < 20; ++i) {
		printf("x, mul, dif = %d, %d, %d\n",
			x, NUM * x, MAX - NUM * x);
		x = 2 * x - (NUM * x * x) / 65536;
	}
}
