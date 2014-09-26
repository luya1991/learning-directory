#include<stdio.h>
#include<function.h>

int main()
{
    function();
    printf("static int value: %d\n",value);

    test testclass1,testclass2;

    testclass1._svalue = 20;

    printf("testclass1._svalue: %d\n",testclass1._svalue);
    printf("testclass2._svalue: %d\n",testclass2._svalue);
    //静态成员属性在该类的所有对象中只保存一份拷贝！！一改全都改！！

    testclass1._dvalue = 40;
    testclass2._dvalue = 80;
    //静态成员函数不允许访问类中的非静态成员，所以function不能访问dvalue

    testclass1.function();
    testclass2.function();

    testclass1.function(40);
    testclass2.function(80);
    //静态成员函数

    return 0;
}
