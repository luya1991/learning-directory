#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <queue>
#include <pthread.h>

using namespace std;

class threadingctrl
{
public:
    void init(bool fileflag);
    bool thread_creating_permit(unsigned int msg_count);
    void thread_creating_info(pthread_t th_id, int err_id);
    void thread_exiting_info();
    queue<pthread_t> threadqueue;
    unsigned int msg_amount;
    unsigned int max_thread;
    unsigned int file_thread_start_count;
    unsigned int file_thread_fail_count;
    bool isfile;
};

// Note: after making the threading control function to work
// well, i removed all the printing-messages.

void threadingctrl::init(bool fileflag)
{
    msg_amount = 100;
    file_thread_start_count = 0;
    file_thread_fail_count = 0;
    isfile = fileflag;
    if(isfile)
        max_thread = 1;
    else
        max_thread = 10;
}

void threadingctrl::thread_creating_info(pthread_t th_id, int err_id)
{
    if(err_id==0)
    {
        //cout<<"a thread has been created successfully!!"<<endl;
        if(isfile)
        {
            threadqueue.push(th_id);
            file_thread_start_count++;
        }
        else
            threadqueue.push(th_id);
        //cout<<"number of running threads: "<<threadqueue.size()<<endl;
    }
    else
    {
        cout<<"thread creating failed!!"<<endl;
        cout<<"trying again...";
    }
}

void threadingctrl::thread_exiting_info()
{
    //cout<<"a thread has finished its task, exiting..."<<endl;
    threadqueue.pop();
    //cout<<"number of running threads: "<<threadqueue.size()<<endl;
}

bool threadingctrl::thread_creating_permit(unsigned int msg_count)
{
    if(!isfile)
    {
        if((msg_count+threadqueue.size()<msg_amount)&&(threadqueue.size()<max_thread))
            return true;
        else
        {
            //cout<<"thread creation being rejected!!"<<endl;
            return false;
        }
    }
    else
    {
        if((file_thread_start_count<max_thread)|((file_thread_start_count>=max_thread)&&(file_thread_start_count-file_thread_fail_count<max_thread)))
            return true;
        else
        {
            //cout<<"thread creation being rejected!!"<<endl;
            return false;
        }
    }
}

