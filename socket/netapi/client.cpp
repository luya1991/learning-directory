#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <map>

#include <stdlib.h>
// sleep, exit

#include <arpa/inet.h>
// inet_aton, inet_ntoa, inet_addr, inet_pton, inet_ntop

#include <netinet/in.h>
// sockaddr_in, htonl, htons, ntohl, ntohs

#include <sys/socket.h>
// socket, connect, bind, listen, accept

#include <sys/ioctl.h>
// ioctl()

#include <net/if.h>
// ifconf, ifreq

using namespace std;

#define SERVERADDR "127.0.0.1"
#define SERVERPORT 10003
#define MAX_LISTEN_QUEUE 10

#include "netapi.cpp"

/**************2014-1-7 class client : public netapi***************/

class client : public netapi
{
public:
    client(string _processname);
    bool client_startup();
    void client_heartbeat();
    void client_sendmsg_to(string m_content,string to_procname);
    void client_start_heartbeat_thread();

    /*****************2014-2-11 new implementation*****************/
    void client_reg();
    void client_dereg();
    void client_close();

private:
    int interval;
};

client::client(string _processname):netapi(_processname)
{
    if(client_startup())
        client_start_heartbeat_thread();
}
bool client::client_startup()
{
    bool flag;
    writeaddrinfo(remoteaddr);
    flag = connectrequest(localsock, remoteaddr);
    client_reg();
    return flag;
}
void client::client_heartbeat()
{
    while(1)
    {
        // heart-beat time interval(10 sec)
        interval = 10;
        sleep(interval);
        string m_type = "hrt";
        string m_content = "this is a heart msg";
        string to_procname = "server1";
        sendmsg(m_type,m_content,to_procname);
    }
}
void * heartthreadcalling(void * arg)
{
    client * threadarg = (client*) arg;
    threadarg->client_heartbeat();
    pthread_exit(NULL);
}
void client::client_start_heartbeat_thread()
{
    pthread_t thread_id;
    int error_id;
    error_id = pthread_create(&thread_id,NULL,heartthreadcalling,this);
    if(error_id==0)
        cout<<"DEBUG INFO: client::startheartthread() is ok!"<<endl;
}
/********************2014-2-11 new implementation*********************/
void client::client_reg()
{
    string m_type = "reg";
    string m_content = "this is a reg msg";
    string to_procname = "server1";
    sendmsg(m_type,m_content,to_procname);
}
void client::client_dereg()
{
    string m_type = "deg";
    string m_content = "this is a deg msg";
    string to_procname = "server1";
    sendmsg(m_type,m_content,to_procname);
}
void client::client_sendmsg_to(string m_content,string to_procname)
{
    string m_type = "nor";
    sendmsg(m_type,m_content,to_procname);
}
