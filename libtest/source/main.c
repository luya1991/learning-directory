#include<stdio.h>
#include<stdlib.h>

//#include"libtest.h"
//不用包含libtest.h也能正常执行，因为执行过程会转到执行.so文件！！
//这就是include和lib的区别，include属于编译时添加包含，lib属于执行时转到！！
//一个是静态添加，一个是有调用到时的添加！！

int add(int a, int b);
int sub(int a, int b);
void hello();

int main(int argc,char** argv)
{
	if(argc<2)
	{
        	printf("usage: <./libtest int1 int2>\n");
		exit(1);
	}
  	printf("Greetings: ");
	hello();
    	int a = atoi(argv[1]);
    	int b = atoi(argv[2]);
    	int result;
	result = add(a,b);
	printf("add result: %d\n",result);
	result = sub(a,b);
	printf("sub result: %d\n",result);
	exit(0);
}
