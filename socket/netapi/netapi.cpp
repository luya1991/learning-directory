#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <map>

#include <stdlib.h>
// sleep, exit

#include <string.h>
// bzero,memset

#include <arpa/inet.h>
// inet_aton, inet_ntoa, inet_addr, inet_pton, inet_ntop

#include <netinet/in.h>
// sockaddr_in, htonl, htons, ntohl, ntohs

#include <sys/socket.h>
// socket, connect, bind, listen, accept

using namespace std;

#define SERVERADDR "127.0.0.1"
#define SERVERPORT 10003
#define MAX_LISTEN_QUEUE 10

#include "msg.cpp"

/*********************************************************
//以下程序可以获取所有链接的地址信息：

    #include <sys/ioctl.h>
    //ioctl()
    #include <net/if.h>
    //ifconf, ifreq

    int i=0;
    struct ifconf ifconf1;
    char buf[512];
    struct ifreq *ifreq1;

    ifconf1.ifc_len = 512;
    ifconf1.ifc_ifcu.ifcu_buf = buf;

    //获取所有接口信息
    ioctl(localsock, SIOCGIFCONF, &ifconf1);

    //接下来一个一个的获取IP地址
    ifreq1 = (struct ifreq*)buf;
    for(i=ifconf1.ifc_len/sizeof(struct ifreq); i>0; i--)
    {
        if(ifreq1->ifr_ifru.ifru_flags == AF_INET)
            {
                cout<<"link name: "<<ifreq1->ifr_ifrn.ifrn_name<<endl;
                cout<<"local addr: "<<inet_ntoa(((struct sockaddr_in*)&(ifreq1->ifr_ifru.ifru_addr))->sin_addr)<<endl;
                ifreq1++;
            }
    }
    //运行结果：
    link name: lo local addr: 127.0.0.1
    link name: wlan0 local addr: 192.168.1.105
  ************************************************************/

/***************************************************************
2013-12-28
    i think that i can define 2 classes:

    class client:public netapi
    class server:public netapi

    the fucking problem is that i do not know how to use thread
    in this project, i need to find out!!
****************************************************************
2014-1-3
   不能用char*啊，fuck你妈的会段错误！！！操操操！！
****************************************************************
2014-1-4
   问题解决了：采用ostringstream可以连接所有类型的变量组成string类型
   新问题：
   1.让客户端可以实现peer连接，即client-client，
   2.尽快将客户端和服务器写成netapi的派生类，
   3.争取把发送和接收写成两个线程。
****************************************************************/

class netapi
{
public:
    netapi(string _processname);
    // ~netapi();
    // @@@@@@@@@  do not forget to call socket.close()  @@@@@@@@@

    // these are net layer api function
    bool socketstartup(int &sockfd);                        //net
    void writeaddrinfo(sockaddr_in &_addr);                 //net
    bool bindsockaddr(int &sockfd, sockaddr_in &_addr);     //net
    bool listenstartup(int &sockfd, int maxlistenqueue);    //net
    bool connectrequest(int &sockfd, sockaddr_in &_addr);   //net

    void msg_write_ip_port(msg &sendmsg);       //net
    void start_msghandle_thread();              //net
    void handle_msgbody(int &sockfd);           //net
    void handle_msghead(int &sockfd);           //net

    void regist(string procname, int recvsockfd);
    // 2014-2-11 added
    // for "reg" message processing

    // these are upper layer api function
    void sendmsg(string &_msgtype, string &_msgcontent,
                 string &_toprocname);
    void recvmsg();                             //client
    void sendfile();                            //client
    void recvfile();                            //client

public:
    int localsock;                  //net
    queue<int> recvsock;            //net
    map<string,int> regtable;       //net
    // max num is 10.
    // problem:
    // we have to distinguish this 10 connection from each other
    // so we have to build a regist-map at server:

    // ******* void regist(client_processname, recvsock) *******

protected:
    string processname;             //net
    sockaddr_in localaddr;          //net
    sockaddr_in remoteaddr;         //net
};

netapi::netapi(string _processname)
{
    processname = _processname;
    bool success;
    success = socketstartup(localsock);
    if(success)
    {
        cout<<"DEBUG INFO: netapi::socketstartup() is ok!"<<endl;
    }
}

bool netapi::socketstartup(int &sockfd)
{
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout<<"socket creating error"<<endl;
        return false;
    }
    cout<<"socket creating success"<<endl;
    return true;
}
void netapi::writeaddrinfo(sockaddr_in &_addr)
{
    _addr.sin_family = AF_INET;
    _addr.sin_addr.s_addr = inet_addr(SERVERADDR);
    _addr.sin_port = htons(SERVERPORT);
    bzero(&(_addr.sin_zero),8);
}
bool netapi::connectrequest(int &sockfd, sockaddr_in &_addr)
{
    if(connect(sockfd,(struct sockaddr*)&_addr,sizeof(_addr))==-1)
    {
        cout<<"connecting server error"<<endl;
        return false;
    }
    cout<<"connecting server success"<<endl;
    return true;
}
bool netapi::bindsockaddr(int &sockfd, sockaddr_in &_addr)
{
    if(bind(sockfd,(struct sockaddr*)&_addr,sizeof(_addr))==-1)
    {
        cout<<"binding error"<<endl;
        return false;
    }
    cout<<"binding socket && addrinfo success"<<endl;
    return true;
}
bool netapi::listenstartup(int &sockfd, int maxlistenqueue)
{
    if(listen(sockfd, maxlistenqueue) == -1)
    {
        cout<<"listening error"<<endl;
        return false;
    }
    cout<<"server is now listening..."<<endl;
    return true;
}
void netapi::handle_msghead(int &sockfd)
{
    char head_buf[60];
    bzero(head_buf,sizeof(head_buf));
    int recvbytes;
    stringstream srcip,destip,srcport,destport,
            isfile,m_type,procname,to_procname;
    if ((recvbytes = recv(sockfd,head_buf,sizeof(head_buf),0))>0)
    {
        cout<<"---------------recv a msg------------------"<<endl;
        cout<<"#DEBUG# message head information: "<<endl;
        //cout<<head_buf<<endl;     ------原始消息头格式

        int i;
        for(i=0;i<15;i++)
        {
            if(head_buf[i]=='#')
                break;
            srcip<<head_buf[i];
        }
        cout<<"#DEBUG# src_ip: "<<srcip.str()<<endl;

        for(i=15;i<20;i++)
        {
            srcport<<head_buf[i];
        }
        cout<<"#DEBUG# src_port: "<<srcport.str()<<endl;

        for(i=20;i<35;i++)
        {
            if(head_buf[i]=='#')
                break;
            destip<<head_buf[i];
        }
        cout<<"#DEBUG# dest_ip: "<<destip.str()<<endl;

        for(i=35;i<40;i++)
        {
            destport<<head_buf[i];
        }
        cout<<"#DEBUG# dest_port: "<<destport.str()<<endl;

        isfile<<head_buf[40];
        cout<<"#DEBUG# isfile: "<<isfile.str()<<endl;
        //2014-2-12
        //这里需要做判断，判断是否是文件，进而进行不同的操作：(msg/file)
        //不过我还没写file部分，这是下一阶段的任务。

        for(i=41;i<49;i++)
        {
            if(head_buf[i]=='#')
                break;
            procname<<head_buf[i];
        }
        cout<<"#DEBUG# src_procname: "<<procname.str()<<endl;

        for(i=49;i<57;i++)
        {
            if(head_buf[i]=='#')
                break;
            to_procname<<head_buf[i];
        }
        cout<<"#DEBUG# dest_procname: "<<to_procname.str()<<endl;
        //2014-2-15
        //这里要根据目的通信进程名称(to_procname)做判断，是否进行转发！！
        //这一条是peer工作的关键！！

        for(i=57;i<60;i++)
        {
            m_type<<head_buf[i];
        }
        cout<<"#DEBUG# msg_type: "<<m_type.str()<<endl;

        if(m_type.str()=="reg")
        {
            regist(procname.str(),recvsock.back());
        }

        /*********************************************************
        2014-2-12
          在这个netapi程序里，我只是在应用层进行了模拟的IP封装与解封装，并没
        用真正意义上了解到IP协议的封装与解封装过程，下一步的学习思路是，写好消
        息服务器以后，开始找网上的IP协议栈的具体实现来阅读！！
          最近两天的工作是先把netapi的reg,dereg,peer功能写好！！
        *********************************************************/

        if(to_procname.str()==processname)
        {
            bzero(head_buf, sizeof(head_buf));
            handle_msgbody(sockfd);
        }
        if(to_procname.str()!=processname)
        {
            int _recvbytes;
            char body_buf[40];
            char trans_buf[100];
            bzero(body_buf,sizeof(body_buf));
            bzero(trans_buf,sizeof(trans_buf));

            cout<<"breakpoint1: "<<head_buf<<endl;
            // head_buf's value is correct here,
            // but it gonna change in the next step..

            if((_recvbytes = recv(sockfd,body_buf,sizeof(body_buf),0))>0)
            {

                // 2014-2-16
                // following codes have got some damn bug!!
                // I don't know why the buf's value just fucking change!!

                cout<<"debugger(1)h: "<<head_buf<<endl;
                cout<<"debugger(1)b: "<<body_buf<<endl;
                cout<<"debugger(1)t: "<<trans_buf<<endl;
                sleep(1);
                memcpy(&trans_buf[0],head_buf,60);
                cout<<"debugger(2)h: "<<head_buf<<endl;
                cout<<"debugger(2)b: "<<body_buf<<endl;
                cout<<"debugger(2)t: "<<trans_buf<<endl;
                sleep(1);
                memcpy(&trans_buf[60],body_buf,40);
                cout<<"debugger(3)h: "<<head_buf<<endl;
                cout<<"debugger(3)b: "<<body_buf<<endl;
                cout<<"debugger(3)t: "<<trans_buf<<endl;
                sleep(1);

                map<string,int>::iterator iter;
                int trans_sock=0;
                for (iter=regtable.begin();iter!=regtable.end();iter++)
                {
                    if(iter->first==to_procname.str())
                    {
                        trans_sock = iter->second;
                        break;
                    }
                }
                if((send(trans_sock,trans_buf,sizeof(trans_buf),0))==-1)
                {
                    cout<<"DEBUG INFO: netapi::trans error!"<<endl;
                }
                else
                    cout<<"DEBUG INFO: netapi::trans is ok!"<<endl;
                bzero(head_buf, sizeof(head_buf));
                bzero(body_buf, sizeof(body_buf));
            }
        }
    }
    if (recvbytes == -1)
    {
        cout<<"receiving msg-head error"<<endl;
        exit(-1);
    }
}
void netapi::handle_msgbody(int &sockfd)
{
    char buf[40];
    bzero(buf,sizeof(buf));
    int recvbytes;
    if ((recvbytes = recv(sockfd, buf, sizeof(buf), 0)) > 0)
    {
        cout<<"#DEBUG# message content: ";
        //cout<<buf<<endl;     ------原始消息体格式
        int i;
        for(i=0;i<40;i++)
        {
            if(buf[i]=='#')
                break;
            cout<<buf[i];
        }
        cout<<endl;
        bzero(buf,sizeof(buf));
        cout<<"---------------end of msg------------------"<<endl;
    }
//--------------------------------------------------------------------
    //*****2014-2-15测试代码*****
    //测试对象：
    //recvsock.back收发功能
    //功能描述：
    //测试一个datasocket是不是同时具有收发功能，还是需要两个sock一发一收。
    /*
    if((send(recvsock.back(),"hello",5,0))==-1)
    {
        cout<<"-------------------"<<endl;
        cout<<"TEST_CODE: ERROR!!"<<endl;
        cout<<"-------------------"<<endl;
        exit(-1);
    }
    */
    //测试结果：
    //datasock同时具有收发功能，不需要两个sock，
    //只要client与server之间存在连接，
    //就可以直接使用recvsock.back来给client2,client3等等发送信息。
//--------------------------------------------------------------------
    if (recvbytes == -1)
    {
        cout<<"receiving msg-body error"<<endl;
        exit(-1);
    }
}
void * msghandlecalling(void * arg)
{
    netapi * threadarg = (netapi*) arg;
    int sockfd = threadarg->recvsock.back();
    while(1)
    {
        threadarg->handle_msghead(sockfd);
    }
    pthread_exit(NULL);
}
void netapi::start_msghandle_thread()
{
    pthread_t thread_id;
    int error_id;
    error_id = pthread_create(&thread_id,NULL,msghandlecalling,this);
    if(error_id==0)
        cout<<"DEBUG INFO: netapi::startmsghandlethread() is ok!"
           <<endl;
}
void netapi::msg_write_ip_port(msg &sendmsg)
{
    struct sockaddr_in localaddr,peeraddr;
    socklen_t len=sizeof(struct sockaddr_in);

    char buf[30];
    memset(buf,0,sizeof(buf));

    bzero(&localaddr,sizeof(localaddr));
    getsockname(localsock,(struct sockaddr*)&localaddr,&len);
    // ******************************************************
    // getsockname() is used to get local ip and local port!!
    // ******************************************************
    cout<<"local ip is "
       <<inet_ntop(AF_INET,&localaddr.sin_addr,buf,sizeof(buf))<<endl;
    cout<<"local port is "
       <<ntohs(localaddr.sin_port)<<endl;
    sendmsg.from_ip =
            inet_ntop(AF_INET,&localaddr.sin_addr,buf,sizeof(buf));
    sendmsg.from_port = ntohs(localaddr.sin_port);

    bzero(&peeraddr,sizeof(peeraddr));
    getpeername(localsock,(struct sockaddr*)&peeraddr,&len);
    // ******************************************************
    // getpeername() is used to get peer ip and peer port!!
    // ******************************************************
    cout<<"peer ip is "
       <<inet_ntop(AF_INET,&peeraddr.sin_addr,buf,sizeof(buf))<<endl;
    cout<<"peer port is "
       <<ntohs(peeraddr.sin_port)<<endl;
    sendmsg.to_ip = SERVERADDR;
    sendmsg.to_port = (unsigned short)SERVERPORT;
}
void netapi::sendmsg(string &_msgtype, string &_msgcontent,
                     string &_toprocname)
{
    msg msg1;
    msg_write_ip_port(msg1);
    msg1.ip_padding(msg1.from_ip);
    msg1.ip_padding(msg1.to_ip);
    msg1.from_processname = processname;
    msg1.name_padding(msg1.from_processname);
    msg1.to_processname = _toprocname;
    msg1.name_padding(msg1.to_processname);
    msg1.msgtype = _msgtype;
    msg1.msg_content = _msgcontent;
    msg1.body_padding(msg1.msg_content);
    string serialstr;
    serialstr = msg1.serialize();
    if(send(localsock, serialstr.c_str(),serialstr.size(), 0) == -1)
    {
        cout<<"sending "<<_msgtype<<" message error"<<endl;
        exit(-1);
    }
    cout<<"sending "<<_msgtype<<" message succeeded!!"<<endl;
}
void netapi::regist(string procname, int recvsockfd)
{
    regtable.insert(pair<string,int>(procname,recvsockfd));
    cout<<"*************************************************"<<endl;
    cout<<"DEBUG INFO: netapi::regist() show map elements..."<<endl;
    map<string,int>::iterator iter;
    for (iter=regtable.begin();iter!=regtable.end();iter++)
    {
        cout<< "("<<iter->first<<","<<iter->second<<")"<< endl;
    }
    cout<<"DEBUG INFO: regist() is ok!"<<endl;
    cout<<"*************************************************"<<endl;
}
