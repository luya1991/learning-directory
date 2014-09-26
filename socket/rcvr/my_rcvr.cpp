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

#include "../api/threading_ctrl.cpp"
#include "../api/file_ctrl.cpp"

#define MAX_LISTEN_QUEUE 20
#define FILEFLAG 1

string path = "../../2.exe";
bool filefinished = false;

using namespace std;
// cout

int localsock;
sockaddr_in local_addr;
// global server listening socket basic information

unsigned int msg_count=0;
// global received-message counting

threadingctrl th_ctrl;
// global threading control class

pthread_mutex_t mutex;
// global visit-lock

filectrl f_ctrl(path);
// global file transition information class

void * receiving_thread(void * arg)
{
    sockaddr_in client_addr;
    char * client_ip;
    unsigned int addrsize;
    int recvbytes;
    char buf[80];
    bzero(buf, sizeof(buf));
    int thread_sock;
    addrsize = sizeof(struct sockaddr_in);

    if ((thread_sock = accept(localsock, (struct sockaddr *) &client_addr, &addrsize)) == -1)
    {
        cout<<"thread_accepting a client error"<<endl;
    }
    client_ip = inet_ntoa(client_addr.sin_addr);
    pthread_mutex_lock(&mutex);
    cout<<"received a connection from: "<<client_ip<<endl;
    pthread_mutex_unlock(&mutex);

    if(FILEFLAG)
    {
        cout<<"now receiving file from client..."<<endl;
        char recvbuff[1];
        bzero(recvbuff,sizeof(recvbuff));
        while((recvbytes = recv(thread_sock, recvbuff, 1, 0)) > 0 )
        {
            f_ctrl.filecontent.write(recvbuff,1);
            bzero(recvbuff,sizeof(recvbuff));
        }
        if(recvbytes == -1)
        {
            cout<<"receiving file content failed..."<<endl;
            exit(-1);
        }
        pthread_mutex_lock(&mutex);
        filefinished = true;
        cout<<"receiving file content succeeded!!"<<endl;
        pthread_mutex_unlock(&mutex);
    }

    else
    {
        if ((recvbytes = recv(thread_sock, buf, sizeof(buf), 0)) == -1)
        {
            cout<<"receiving message error"<<endl;
            exit(-1);
        }
        pthread_mutex_lock(&mutex);
        msg_count++;
        cout<<"received "<<msg_count<<" message from client..."<<endl;
        cout<<"message content: ";
        cout<<buf<<endl;
        pthread_mutex_unlock(&mutex);
    }

    free(arg);
    close(thread_sock);
    pthread_mutex_lock(&mutex);
    th_ctrl.thread_exiting_info();
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main()
{
    // int socket(int domain, int type, int protocol);

    if ((localsock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout<<"socket creating error"<<endl;
        exit(-1);
    }
    cout<<"socket creating succeeded!!"<<endl;

    // int bind(int sockfd,struct sockaddr *addr, int addrlen);

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    local_addr.sin_port = htons(5000);
    bzero(&(local_addr.sin_zero),8);

    if (bind(localsock, (struct sockaddr*) &local_addr, sizeof(local_addr)) == -1)
    {
        cout<<"binding error"<<endl;
        exit(-1);
    }
    cout<<"binding socket && addrinfo succeeded!!"<<endl;

    // int listen(int sockfd， int backlog);

    if (listen(localsock, MAX_LISTEN_QUEUE) == -1)
    {
        cout<<"listening error, or services out of range"<<endl;
        exit(-1);
    }
    cout<<"server is now listening..."<<endl;

    // accept(), recv() shoud all be put into the thread!!

    pthread_mutex_init(&mutex,NULL);

    if(FILEFLAG)
        th_ctrl.init(true);
    else
        th_ctrl.init(false);

    while(1)
    {
        if(th_ctrl.threadqueue.size()<th_ctrl.max_thread)
        {
            pthread_t thread_id;
            int err_id;
            pthread_mutex_lock(&mutex);
            err_id = pthread_create(&thread_id,NULL,receiving_thread,NULL);
            th_ctrl.thread_creating_info(thread_id,err_id);
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            sleep(1);
            if(f_ctrl.filecontent.peek()!=EOF)
                f_ctrl.routinecheck();
            /*
            cout<<"running threads have reached the maximum number!!"<<endl;
            cout<<"waiting for a thread to finish its work..."<<endl;
            cout<<"the sleep code has been triggered..."<<endl;
            sleep(5);
            */
        }
    }

    sleep(1000);
    cout<<"maybe this will never be shown on the terminal but, ";
    cout<<"this server is tired now..."<<endl;
    cout<<endl<<"exit receiving task..."<<endl<<endl;
    close(localsock);
    return 0;
}
