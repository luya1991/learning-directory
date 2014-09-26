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

#include <unistd.h>
// sleep usleep

#define MSG_AMOUNT 10000

using namespace std;
// cout

int localsock;
sockaddr_in to_addr;
// global client sending socket basic information

unsigned int msg_count=0;
// global sent-message counting

int main()
{
    if ((localsock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout<<"sending socket creating error"<<endl;
        exit(-1);
    }
    cout<<"sending socket creating succeeded!!"<<endl;

    to_addr.sin_family = AF_INET;
    to_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    to_addr.sin_port = htons(9887);
    bzero(&(to_addr.sin_zero),8);

    if(connect(localsock, (struct sockaddr*) &to_addr, sizeof(to_addr)) == -1)
    {
        cout<<"connecting error"<<endl;
        exit(-1);
    }
    cout<<"connecting sever succeeded!!"<<endl;

    struct timeval start;
    struct timeval end;
    double run_time;

    int sendbytes;
    gettimeofday(&start,NULL);
    while(msg_count<MSG_AMOUNT)
    {
        if((sendbytes = send(localsock, "Hello server!!", 15, 0)) == -1)
        {
            cout<<"sending message error"<<endl;
            exit(-1);
        }

        else if(sendbytes > 0)
        {
            cout<<"sending message succeeded!!"<<endl;
            msg_count = msg_count+1;
            cout<<"msg-sent number: "<<msg_count<<endl;
        }
    }

    close(localsock);
    gettimeofday(&end,NULL);
    run_time = end.tv_sec - start.tv_sec + (double)(end.tv_usec-start.tv_usec)/1000000.0;
    cout<<endl<<"real time of sending task is: "<<run_time<<"sec"<<endl;
    cout<<endl<<"exit sending task..."<<endl<<endl;
    return 0;
}

