#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

#include <string.h>
// bzero, bcopy, bcmp, memset, memcpy, memcmp

#include <pthread.h>
// pthread_t, pthread_create, pthread_exit

#include <sys/time.h>
// timeval

using namespace std;
// fstream

//NOTE:
//    THIS CLASS CAN BE CHANGED BETTER BY PUBLIC INHERITING "fstream"
//    BUT THERE ARE TOO MANY PROBLEMS IN INHERITING, I HAVE TRIED THAT!!

class filectrl
{
public:
    filectrl(string path);
    ~filectrl();
    void computefilesize();
    void showfilesize();
    void getfilesentpercent();
    void getsentspeed(double last);
    void updatelastpos();
    void testmoving();
    void routinecheck();

    double filesize;
    double lastpos;
    fstream filecontent;
};

filectrl::filectrl(string path)
{
    filecontent.open(path.c_str(),ios::in|ios::out|ios::binary|ios::app);
    if(!filecontent)
    {
        cout<<"file opening error"<<endl;
        exit(-1);
    }
    lastpos = (double) ios::beg;
    computefilesize();
    showfilesize();
}

filectrl::~filectrl()
{
    filecontent.close();
}

void filectrl::computefilesize()
{
    filecontent.seekg(0,ios::end);
    ios::pos_type fileptr_end = filecontent.tellg();
    filesize = (double)fileptr_end;
    filecontent.seekg(0,ios::beg);
}

void filectrl::showfilesize()
{
    cout<<"filesize: "<<filesize/1024.0/1024.0<<" MB"<<endl;
}

void filectrl::getfilesentpercent()
{
    double percent;
    double sentsize;
    ios::pos_type fileptr_current = filecontent.tellg();
    sentsize = (double)fileptr_current;
    // cout<<"current ptr: "<<sentsize<<endl;
    percent = 100*sentsize/(this->filesize);
    cout<<"sent percent: "<<percent<<"%"<<endl;
}

void filectrl::getsentspeed(double last)
{
    double size;
    // cout<<"last:"<<last<<" cur:"<<objectfile.tellg()<<endl;
    size = ((double)filecontent.tellg()-last)/1024.0/1024.0;
    cout<<"speed: "<<size<<" MB/s"<<endl;
}

void filectrl::updatelastpos()
{
    lastpos = (double)filecontent.tellg();
}

void filectrl::testmoving()
{
    cout<<"now move 150 for testing..."<<endl;
    if(filecontent.peek()!=EOF)
        filecontent.seekg(150,ios::cur);
    else
    {
        cout<<"file: out of range!!"<<endl;
        exit(-1);
    }
}

void filectrl::routinecheck()
{
    getfilesentpercent();
    getsentspeed(lastpos);
    updatelastpos();
}
