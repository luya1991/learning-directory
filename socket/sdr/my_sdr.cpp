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

#define MSG_AMOUNT 100
#define FILEFLAG 1

string path = "../../1.exe";
bool filefinished = false;

using namespace std;
// cout

int localsock;
sockaddr_in to_addr;
// global client sending socket basic information

unsigned int msg_count=0;
// global sent-message counting

threadingctrl th_ctrl;
// global threading control information class

pthread_mutex_t mutex;
// global visit-lock

filectrl f_ctrl(path);
// global file transition information class

void * sending_thread(void * arg)
{
    if(FILEFLAG)
    {
        cout<<"now sending file to server..."<<endl;
        char sendbuff[1];
        bzero(sendbuff,sizeof(sendbuff));

        while(f_ctrl.filecontent.peek()!= EOF)
        {
            f_ctrl.filecontent.read(sendbuff,1);
            if(send(localsock, sendbuff, 1, 0) == -1)
            {
                cout<<"sending file content error"<<endl;
                th_ctrl.file_thread_fail_count++;
                exit(-1);
            }
            bzero(sendbuff,sizeof(sendbuff));
        }
        pthread_mutex_lock(&mutex);
        cout<<"sent percent: 100%"<<endl;
        cout<<"sending file content succeeded!!"<<endl;
        filefinished = true;
        pthread_mutex_unlock(&mutex);
    }

    else
    {
        if(send(localsock, "Hello server!!", 15, 0) == -1)
        {
            cout<<"sending message error"<<endl;
            exit(-1);
        }
        pthread_mutex_lock(&mutex);
        cout<<"sending message succeeded!!"<<endl;
        msg_count = msg_count+1;
        cout<<"msg-sent number: "<<msg_count<<endl;
        pthread_mutex_unlock(&mutex);
    }

    free(arg);
    close(localsock);
    pthread_mutex_lock(&mutex);
    th_ctrl.thread_exiting_info();
    pthread_mutex_unlock(&mutex);
    pthread_exit(NULL);
}

int main()
{
    if ((localsock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        cout<<"sending socket creating error"<<endl;
        exit(-1);
    }
    pthread_mutex_lock(&mutex);
    cout<<"sending socket creating succeeded!!"<<endl;
    pthread_mutex_unlock(&mutex);

    to_addr.sin_family = AF_INET;
    to_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    to_addr.sin_port = htons(5000);
    bzero(&(to_addr.sin_zero),8);

    if(connect(localsock, (struct sockaddr*) &to_addr, sizeof(to_addr)) == -1)
    {
        cout<<"connecting error"<<endl;
        exit(-1);
    }
    pthread_mutex_lock(&mutex);
    cout<<"connecting sever succeeded!!"<<endl;
    pthread_mutex_unlock(&mutex);

//  send() should be put into the thread!!

    pthread_mutex_init(&mutex,NULL);

    if(FILEFLAG)
        th_ctrl.init(true);
    else
        th_ctrl.init(false);

    struct timeval start;
    struct timeval end;
    double run_time;

    gettimeofday(&start,NULL);

    if(FILEFLAG)
    {
        while(!filefinished)
        {
            if(th_ctrl.thread_creating_permit(msg_count))
            {
                pthread_t thread_id;
                int err_id;
                pthread_mutex_lock(&mutex);
                err_id = pthread_create(&thread_id,NULL,sending_thread,NULL);
                th_ctrl.thread_creating_info(thread_id,err_id);
                pthread_mutex_unlock(&mutex);
            }
            else
            {
                sleep(1);
                if(f_ctrl.filecontent.peek()!=EOF)
                    f_ctrl.routinecheck();
            }
        }
    }
    else
    {
        while(msg_count<MSG_AMOUNT)
        {
            if(th_ctrl.thread_creating_permit(msg_count))
            {
                pthread_t thread_id;
                int err_id;
                pthread_mutex_lock(&mutex);
                err_id = pthread_create(&thread_id,NULL,sending_thread,NULL);
                th_ctrl.thread_creating_info(thread_id,err_id);
                pthread_mutex_unlock(&mutex);
            }
            else
            {
                /*
                cout<<"active thread num: "<<th_ctrl.threadqueue.size()<<" ";
                cout<<"message sent num: "<<msg_count<<endl;
                cout<<"the sleep code has been triggered..."<<endl;
                sleep(5);
                sleepcount++;
                */
            }
        }
    }

    gettimeofday(&end,NULL);
    run_time = end.tv_sec - start.tv_sec + (double)(end.tv_usec-start.tv_usec)/1000000.0;
    cout<<endl<<"real time of sending task is: "<<run_time<<"sec"<<endl;
    cout<<endl<<"exit sending task..."<<endl<<endl;
    return 0;
}
