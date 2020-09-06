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
