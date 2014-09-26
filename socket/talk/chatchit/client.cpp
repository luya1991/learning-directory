#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_LISTEN_QUEUE 20

using namespace std;

int local_listening_sock;
sockaddr_in local_addr;

pthread_mutex_t mutex;

typedef struct
{
    int recv_sock;
}r_thread_argv;

typedef struct
{
    int sending_sock;
    string sending_content;
}s_thread_argv;

// all the typedef information should be put at the very start!!

void * receiving_thread(void * arg)
{
    r_thread_argv* arg_1 = (r_thread_argv*) arg;
    int receiving_sock = arg_1->recv_sock;

    int recvbytes;
    char buf[80];
    bzero(buf, sizeof(buf));

    while((recvbytes = recv(receiving_sock, buf, sizeof(buf), 0)) >= 0)
    {
        if(recvbytes)
        {
            pthread_mutex_lock(&mutex);
            cout<<"peer client message:"<<endl;
            cout<<buf<<endl;
            bzero(buf,sizeof(buf));
            pthread_mutex_unlock(&mutex);
        }
    }
    if(recvbytes<0)
    {
        cout<<"receiving message error"<<endl;
        exit(-1);
    }

    pthread_exit(NULL);
}

void * sending_thread(void * arg)
{
    s_thread_argv* arg_1 = (s_thread_argv*) arg;
    int sending_sock = arg_1->sending_sock;
    string content = arg_1->sending_content;
    int sendbytes=content.size();

    if(send(sending_sock, content.c_str(), sendbytes, 0) == -1)
    {
        cout<<"sending message error"<<endl;
        exit(-1);
    }

    pthread_exit(NULL);
}

int main()
{
    pthread_mutex_init(&mutex,NULL);

    if ((local_listening_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout<<"listening socket creating error"<<endl;
        exit(-1);
    }
    cout<<"listening socket creating succeeded!!"<<endl;

    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    local_addr.sin_port = htons(5000);
    bzero(&(local_addr.sin_zero),8);

    if (bind(local_listening_sock, (struct sockaddr*) &local_addr, sizeof(local_addr)) == -1)
    {
        cout<<"binding error"<<endl;
        exit(-1);
    }
    cout<<"binding socket && local addrinfo succeeded!!"<<endl;

    if (listen(local_listening_sock, MAX_LISTEN_QUEUE) == -1)
    {
        cout<<"listening error, maybe services out of range"<<endl;
        exit(-1);
    }
    cout<<"this client is now listening..."<<endl;

    cout<<endl<<"now sleep 5 secs waiting for peer start up..."<<endl<<endl;
    sleep(5);

    int sending_sock;
    sockaddr_in to_addr;

    if ((sending_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout<<"sending socket creating error"<<endl;
        exit(-1);
    }
    cout<<"sending socket creating succeeded!!"<<endl;

    to_addr.sin_family = AF_INET;
    to_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    to_addr.sin_port = htons(7000);
    bzero(&(to_addr.sin_zero),8);

    if(connect(sending_sock, (struct sockaddr*) &to_addr, sizeof(to_addr)) == -1)
    {
        cout<<"connecting error"<<endl;
        exit(-1);
    }
    cout<<"connecting peer client succeeded!!"<<endl;

    sockaddr_in client_addr;
    char * client_ip;
    unsigned int addrsize = sizeof(struct sockaddr_in);
    int receiving_sock;

    if ((receiving_sock = accept(local_listening_sock, (struct sockaddr *) &client_addr, &addrsize)) == -1)
    {
        cout<<"accepting peer client error"<<endl;
    }
    client_ip = inet_ntoa(client_addr.sin_addr);
    cout<<"accepting a connection from: "<<client_ip<<endl;

    cout<<endl<<"peer client link established!!"<<endl<<endl;

    r_thread_argv arg_1;
    arg_1.recv_sock = receiving_sock;

    pthread_t receiving_thread_id;
    int rerr_id;
    pthread_mutex_lock(&mutex);
    rerr_id = pthread_create(&receiving_thread_id,NULL,receiving_thread,&arg_1);
    if(rerr_id == -1)
    {
        cout<<"receiving thread creating error"<<endl;
        exit(-1);
    }
    pthread_mutex_unlock(&mutex);

    s_thread_argv arg_2;
    arg_2.sending_sock = sending_sock;

    pthread_t sending_thread_id;
    int serr_id;

    char sendbuf[80];
    bzero(sendbuf,sizeof(sendbuf));

    while(1)
    {
        cin.getline(sendbuf,sizeof(sendbuf));
        arg_2.sending_content = sendbuf;
        if(!arg_2.sending_content.empty())
        {
            serr_id = pthread_create(&sending_thread_id,NULL,sending_thread,&arg_2);
            if(serr_id == -1)
            {
                cout<<"sending thread creating error"<<endl;
                exit(-1);
            }
        }
    }

    close(sending_sock);
    close(local_listening_sock);
    return 0;
}
