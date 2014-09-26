
#include <iostream>

using namespace std;

class Base
{
public:
    void func( )
    {
        cout<<"Base class function."<<endl;
    }
};

class Derived : public Base
{
public:
    void func( )
    {
        cout<<"Derived class function."<<endl;
    }
};

void foo(Base b)
{
    b.func( );
}

int main( )
{
    Derived d;
    Base b;
    Base *p = &d;
    Base &br = d;

    b = d;
    b.func( );
    d.func( );
    p -> func( );
    foo(d);
    br.func( );

    // 2014-2-12
    // 目前我还不是搞得很懂，这个破玩意到底是怎么工作的。。
    // 我打算过段时间再回头看看。

    // 2014-3-30
    // 总结，C++往往允许用派生类变量初始化基类变量，基类指针，基类引用，基类函数参数
    // 但是，C++不允许用基类变量为派生类变量做初始化的行为！！

    return 0;
}
