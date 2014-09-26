#include <iostream>
#include <string.h>
using namespace std;
class testclass
{
    public:

    testclass(int b,const char* cstr)
    {
        a=b;
        str=new char[b];
        strcpy(str,cstr);
    }

    testclass(const testclass& test1)
    {
        a=test1.a;
        str=new char[a];
        //此处为深拷贝
        //若“str = c.str”则为浅拷贝

    //关于深拷贝和浅拷贝提几点注意：

    /*
    在某些状况下，类内成员变量需要动态开辟堆内存（new），如果实行位拷贝，
    也就是把对象里的值完全复制给另一个对象，如A=B，
    这时，如果B中有一个成员变量指针已经申请了内存，
    那A中的那个成员变量也指向同一块内存。
    这就出现了问题：当B把内存释放了（析构），
    这时A内的指针就会变为“野指针”（指针所指位置的数据没有意义），导致运行错误。

    深拷贝和浅拷贝可以简单理解为：如果一个类拥有资源（堆，文件，系统资源），
    当这个类的对象发生复制过程的时候，
    如果有资源重新分配，就是深拷贝，
    反之，如果没有重新分配资源，就是浅拷贝。

    注意：默认的系统拷贝构造函数都是浅拷贝！！
    也就是说，当类拥有资源的时候，必须自己写它的拷贝构造函数！！

    深拷贝使内存中具有两个对象各自所有数据的两份拷贝，而不是共用一份拷贝，
    深拷贝中两个指针指向各自独立的两份数据，即内存中两个不同的位置，
    浅拷贝中两个指针将指向内存中同一个位置。
    */

        if(str!=NULL)
        strcpy(str,test1.str);
    }

    void Show()
    {
        cout<<str<<endl;
    }

    void Delete()
    {
        delete str;
        cout<<str<<endl;
        str = NULL;
        if(str == NULL)
            cout<<"after 'delete' operation"<<endl
               <<"'*str': not exist"<<endl
               <<"'str': still exist, but str=NULL now"<<endl;
        //关于new-delete提几点注意：
        /*
            char* str = new char[x];
            这个语句会存储两个数据：
                1.char[x] 被存储于堆区
                2.str指针 被存储于栈区

            delete str;
            这个语句只会清除堆上的数据，不会清除栈上的数据，

            所以，str所指数据没了，但str本身还存在，变为悬垂指针，危险！！
            因此，此时最好将str设置为NULL！！

            另外： int* pint = new int(20) 和 int* pint = new int[20]
            前者：创建一个int指针，指向的内容为20；后者：创建一个长度20的int数组
        */
    }

    ~testclass()
    {
        //重点注意：delete内容写到析构函数内部和写到析构函数之前有所不同！！
        //再提几点关于delete的几点注意：

        /*
        delete 操作符会被编译器展开为至少两个子过程

        1. 先调用所传入类型的析构函数
           （如果是虚析构函数，之后还会去调用子类的析构函数）

        2. 调用内存销毁函数，释放所传入指针指向的内存

        也就是说，先析构，再释放内存，且析构并不等于释放了内存！！

        对非动态分配的对象调用delete，
        第一个过程是不会有问题的，
        但是第二个过程就会让你的程序挂掉，（根本就是非动态分配的还delete毛线嘛）

        在析构函数中调用delete，会在第一个子过程中形成无限递归，
        不会执行到第二个子过程。

        伪代码清晰的解释了导致递归的原因

        // class base 的析构函数会编译成如下形式
        void base_destructor(T* );

        // 这是delete 展开之后的伪代码
        operator delete(base* p)
        {
            base_destructor(p);
            free(p);
        }

        // base 的析构函数实现
        void base_destructor(T* p)
        {
            delete(p);
        }

        */
    }

    private:

    int a;
    char *str;
};

int main()
{
    testclass A(10,"hello world");
    testclass B=A;
    B.Show();
    B.Delete();
    return 0;
}
