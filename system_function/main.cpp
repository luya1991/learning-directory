#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<wait.h>
#include<errno.h>

int mysystem(const char * cmdstring)
{
    pid_t pid;
    int status;

    if(cmdstring == NULL)
    {
        return (1);
    }

    if((pid = fork())<0)
    {
        status = -1;
    }

    else if(pid == 0)
    {
        execl("/bin/sh", "sh", "-c", cmdstring, (char *)0);
        //一些说明：
        //execl()转而通过sh解析命令并执行子进程，而sh又会创建一个子进程，
        //所以这样获取不到目标程序的cpid，这里其实相当于ccpid，cpid是sh的pid
        _exit(127);
        //若子进程正常执行，则不会执行此语句
    }

    else
    {
        while(waitpid(pid, &status, 0) < 0)
        {
            if(errno != EINTR)
            {
                status = -1;
                break;
            }
        }
    }
    return status;
}

void sigchld_handler(int signo)
{
    pid_t child_pid;
    int state;
    while((child_pid = waitpid(-1, &state, WNOHANG))>0)
    {
        printf("child proc %d has exited!\n",(int)child_pid);
    }
    return;
}

int main(int argc, char** argv)
{
    if(argc!=3)
    {
        printf("usage: ./systemfunction <path/command> <execl argv[0]>\n");
        exit(-1);
    }
    //说明：execl函数需要通过path/command替换自己为另外一个程序，
    //而且不像一般的程序有默认的argv[0]就是自己的名字，它需要被给予argv[0]，
    //而且execl函数的最后一个变量必须是NULL，因此，它至少需要三个输入参数。

    int status=0;
    //status = mysystem(argv[1]);
    //printf("status: %d\n",status);

    pid_t pid;
    signal(SIGCHLD,sigchld_handler);

    while(1)
    {
        printf("**********************************************\n");
        printf("sleep 10 to fork()...\n");
        sleep(10);
        if((pid = fork())<0)
        {
            printf("fork() failed\n");
            status = -1;
        }
        else if(pid == 0)
        {
            printf("###############################################\n");
            printf("this is the child process\n");
            execl(argv[1],argv[2],(char*)0);
            /*  execl函数说明：
                execl(const char * path,const char * arg,...)
                execl()用来执行参数path字符串所代表的文件路径，
                接下来的参数代表执行该文件时传递过去的argv(0)、argv[1]...
                最后一个参数必须用空指针(NULL)作为结束
            */
            _exit(127);
        }
        else
        {
            printf("**********************************************\n");
            printf("fork() success(pid %d),now sleep 10 to kill...\n",pid);
            sleep(10);
            kill(pid,9);
            printf("pid %d has been killed\n",pid);
        }
    }
    return status;
}
