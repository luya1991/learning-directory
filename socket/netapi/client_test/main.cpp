#include"../client.cpp"
#include<stdlib.h>

using namespace std;

int main()
{
    client c1("client1");
    c1.client_sendmsg_to("hello","client2");

    /*client c2("client2");
    int recvbytes;
    char buf[100];
    bzero(buf,sizeof(buf));
    if((recvbytes= recv(c2.localsock, buf, sizeof(buf), 0)) >0)
    {
        cout<<"-------------------"<<endl;
        cout<<"TEST TRANS: OK!!"<<endl;
        cout<<buf<<endl;
        cout<<"-------------------"<<endl;
    }*/
//------------------------------------------------------------------
    //*****2014-2-15测试代码*****
    //测试对象：
    //recvsock.back收发功能
    //功能描述：
    //测试一个datasocket是不是同时具有收发功能，还是需要两个sock一发一收。
    /*
    int recvbytes;
    char buf[5];
    bzero(buf,sizeof(buf));
    if((recvbytes= recv(c1.localsock, buf, sizeof(buf), 0)) >0)
    {
        cout<<"-------------------"<<endl;
        cout<<"TEST CODE: OK!!"<<endl;
        cout<<buf<<endl;
        cout<<"-------------------"<<endl;
    }
    */
    //测试结果：
    //datasock同时具有收发功能，不需要两个sock，
    //只要client与server之间存在连接，
    //就可以直接使用recvsock.back来给client2,client3等等发送信息。
//-----------------------------------------------------------------
    sleep(1000);
    return 0;
}
