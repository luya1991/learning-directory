#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <queue>
#include <map>

using namespace std;

/*****************************************************************
  2014-1-6
    I have found a lot program about this, but I never understand
  anything..fuck..
    what I know is that, this problem is really important, if I
  want to continue writing my own "netapi" class
    damn it, I have to find a way to work this out!!
    (maybe in winter holiday)
******************************************************************
  2014-1-7
    haha, I am a genius!! and now I know how to use member-fuction
  as a thread function!!
    it totally works well!!
    ## WARNING: I have found limit in here:
    ## the thread arguments must be "this"!! says, all args must
    ## be defined as class's member attributes.
*****************************************************************/

class test
{
public:
    test();
    ~test();
public:
    int p;
    void sayHello(int r);
    void createThread();
private:
    int q;
};

test::test()
{}
test::~test()
{}

void *threadFunction(void *arg)
{
    cout<<"now create a thread!!"<<endl;
    test *obj = (test *)arg;
    printf("get p: %d\n",obj->p);
    obj->sayHello(obj->p);
    cout<<"now exit a thread!!"<<endl;
    pthread_exit(NULL);
}

void test::sayHello(int r)
{
    cout<<"now do the function of the thread:"<<endl;
    printf("Hello world %d!\n", r);
}

void test::createThread()
{
    p = 1;
    q = 2;
    pthread_t threadID;
    pthread_create(&threadID, NULL, threadFunction, this);
    //getchar();
}

int main()
{
    test t;
    int i;
    for(i=0;i<5;i++)
    {
        t.createThread();
        sleep(1);
    }

    return 0;
}
