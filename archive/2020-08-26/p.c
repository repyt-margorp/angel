#include <stdio.h>

int len = 0;;
void *pool[1000][2];

void *new_pool()
{
	return pool[len++];
}

void *identity()
{
	void **root;
	void **lmdabs;
	void **funapp;

	root = new_pool();
	lmdabs = new_pool();
	funapp = new_pool();

	root[0] = lmdabs;
	root[1] = funapp;

	funapp[0] = root[0];
	funapp[1] = NULL;

	{
		void **x;

		x = new_pool();
		x[0] = runapp[0]
		lmdabs[0] = new_pool();
		lmdabs[1] = NULL;
		lmbabs[1]
	}

	return root;
}

int list_do(
	void **list,
	void *param,
	int(*func)(void *, void *))
{
	void **temp;

	temp = list;
	while(temp == NULL) {
		func(param, temp[0]);
		temp = temp[1];
	}
}
#if 0
struct print_lambda_abstraction_parameter {
	int length;
};
int print_lambda_abstraction(
	void *param,
	void **list)
{
	struct print_lambda_abstraction_parameter *par;

	par = param;
	par->length++;
	print("%d(%x)", length, );
}
#endif

void print(void *list[])

{
	void **temp;

	if(list == NULL) {
		return;
	}

	if(list[0] != NULL) {
		temp = list[0];
		printf("\\");
		while(temp != NULL) {
			printf(" %x", &temp[0]);
		}
	}

	printf(" . ");

	if(list[1] != NULL) {
		temp = list[1];
		printf("( ");
		while(temp != NULL) {
			printf("%x@%x ", temp[0], &temp[0]);
			temp = temp[1];
		}
		printf(")");
	}
	printf("\n");
}

int main()
{
	void *id;

	printf("hello, world\n");
	id = identity();
	print(id);
}
