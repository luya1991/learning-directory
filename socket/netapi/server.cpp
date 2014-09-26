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

/**************2014-1-7 class server : public netapi***************/

class server : public netapi
{
public:
    server(string _processname);
    bool server_startup();
    void server_start_accept_thread();
    void server_recvprocess();
    void server_handleaccept(int &sockfd);

    /*****************2014-2-11 new implementation*****************/
    void server_close();
    void server_routinechek();

private:
    map<string,int> regist_table;
};

server::server(string _processname):netapi(_processname)
{
    if(server_startup())
        server_start_accept_thread();
}
bool server::server_startup()
{
    bool flag;
    writeaddrinfo(localaddr);
    flag = bindsockaddr(localsock, localaddr);
    flag = listenstartup(localsock, MAX_LISTEN_QUEUE);
    return flag;
}
void * recvprocesscalling(void * arg)
{
    server * threadarg = (server*) arg;
    threadarg->server_recvprocess();
    pthread_exit(NULL);
}
void server::server_start_accept_thread()
{
    pthread_t thread_id;
    int error_id;
    error_id = pthread_create(&thread_id,NULL,recvprocesscalling,this);
    if(error_id==0)
        cout<<"DEBUG INFO: server::startacceptthread() is ok!"<<endl;
}
void server::server_recvprocess()
{
    while(1)
    {
        server_handleaccept(localsock);
    }
}
void server::server_handleaccept(int &sockfd)
{
    sockaddr_in _addr;
    unsigned int addrsize = sizeof(_addr);
    char * client_ip;
    int new_sock;
    if((new_sock = accept(sockfd, (struct sockaddr *)&_addr, &addrsize)) == -1)
    {
        cout<<"accepting client error"<<endl;
    }
    client_ip = inet_ntoa(_addr.sin_addr);
    cout<<"received a connection from: "<<client_ip<<endl;
    recvsock.push(new_sock);
    cout<<"#DEBUG# recv sock now: "<<recvsock.back()<<endl;
    start_msghandle_thread();
}
