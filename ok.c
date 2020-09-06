#include <stdio.h>

struct divide_return {
	int q;
	int r;
};
struct divide_return divide(int a, int b)
{
	struct divide_return ret;
	ret.q = a / b;
	ret.r = a % b;
	return ret;
}
int main()
{
	struct divide_return ret;
	ret = divide(3, 4);
	printf("q, r = %d, %d\n", ret.q, ret.r);
}
