#ifndef FUNCTION_H
#define FUNCTION_H

#include<stdio.h>

static int value = 10;

static void function()
{
    printf("this is a static void function\n");
}

class test
{
public:
    test()
    {
        printf("constructor\n");
    }
    ~test()
    {
        printf("destructor\n");
    }

    static int _svalue;
    int _dvalue;

    static void function(int j=0)
    {

        /*
            printf("%d\n",this->_svalue);
            错误！静态成员函数内不允许引用this指针！！
        */

        int i;
        /*
            i = _dvalue;
            错误！静态成员函数不允许访问类的非静态成员！
        */
        i = _svalue;
        if(j)
            printf("there is an input! int-value: %d\n",j);
        else
            printf("there is no input! svalue: %d\n",i);
    }
};

int test::_svalue = 10;
//必须对定义的静态成员变量在本文件作用域内作初始化！

#endif // FUNCTION_H
