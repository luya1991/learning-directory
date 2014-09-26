#include<iostream>
using namespace std;

class ExceptionClass

{
    string name;

public:

    ExceptionClass(string name = "default name")
    {
             cout<<"Constructor "<<name<<endl;
             this->name=name;
    }

    ~ExceptionClass()
    {
             cout<<"Destructor "<<name<<endl;
    }

    void mythrow()
    {
            cout<<"throw an exception in mythrow..."<<endl;
            throw ExceptionClass("my throw");
    }
};

int main()
{
    ExceptionClass e("Test");
    try
    {
        e.mythrow();
    }
    catch(...)
    {
      cout<<"catch an exception from throw()!!"<<endl;
    }

}
