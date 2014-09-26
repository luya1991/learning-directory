#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <string.h>
// bzero, bcopy, bcmp, memset, memcpy, memcmp

#include <arpa/inet.h>
// inet_aton, inet_ntoa, inet_addr, inet_pton, inet_ntop

#include <netinet/in.h>
// sockaddr_in, htonl, htons, ntohl, ntohs

#include <sys/socket.h>
// socket, connect, bind, listen, accept

#include <pthread.h>
// pthread_t, pthread_create, pthread_exit

#include <sys/time.h>
// timeval

using namespace std;
// cout

#define MAX_LISTEN_QUEUE 20
// listenfd backlog

int localsock;
sockaddr_in local_addr;
// global server listening socket basic information

unsigned int msg_count=0;
// global received-message counting

int main()
{
    if ((localsock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout<<"socket creating error"<<endl;
        exit(-1);
    }
    cout<<"socket creating succeeded!!"<<endl;

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    local_addr.sin_port = htons(9887);
    bzero(&(local_addr.sin_zero),8);

    if (bind(localsock, (struct sockaddr*) &local_addr, sizeof(local_addr)) == -1)
    {
        cout<<"binding error"<<endl;
        exit(-1);
    }
    cout<<"binding socket && addrinfo succeeded!!"<<endl;

    if (listen(localsock, MAX_LISTEN_QUEUE) == -1)
    {
        cout<<"listening error, or services out of range"<<endl;
        exit(-1);
    }
    cout<<"server is now listening..."<<endl;

    sockaddr_in client_addr;
    char * client_ip;
    unsigned int addrsize;
    int recvbytes;
    char buf[15];
    bzero(buf, sizeof(buf));
    int recv_sock;
    addrsize = sizeof(struct sockaddr_in);

    if ((recv_sock = accept(localsock, (struct sockaddr *) &client_addr, &addrsize)) == -1)
    {
        cout<<"thread_accepting a client error"<<endl;
    }
    client_ip = inet_ntoa(client_addr.sin_addr);
    cout<<"received a connection from: "<<client_ip<<endl;

    while(1)
    {
        if ((recvbytes = recv(recv_sock, buf, sizeof(buf), 0)) == -1)
        {
            cout<<"receiving message error"<<endl;
            exit(-1);
        }
        else if(recvbytes > 0)
        {
            msg_count++;
            cout<<"received "<<msg_count<<" message from client..."<<endl;
            //cout<<"message content: ";
            //cout<<buf<<endl;
            bzero(buf,sizeof(buf));
        }
    }
    sleep(1000);
    cout<<"maybe this will never be shown on the terminal but, ";
    cout<<"this server is tired now..."<<endl;
    cout<<endl<<"exit receiving task..."<<endl<<endl;
    close(localsock);
    return 0;
}

