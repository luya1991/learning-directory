#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/time.h>
#include<sys/resource.h>

int main(int argc, char** argv)
{
    struct rusage myusage, childusage;
    int res;

    res = system("find / genius");
    printf("res: %d\n",res);

    if(getrusage(RUSAGE_CHILDREN,&myusage)<0)
    {
        printf("error in getrusage()\n");
        exit(1);
    }

    if(getrusage(RUSAGE_SELF,&childusage)<0)
    {
        printf("error in getrusage()\n");
        exit(1);
    }

    double usertime,systime;
    usertime = (double)myusage.ru_utime.tv_sec
            +myusage.ru_utime.tv_usec/1000000.0;
    usertime += (double)childusage.ru_utime.tv_sec
            +childusage.ru_utime.tv_usec/1000000.0;
    systime = (double)myusage.ru_stime.tv_sec
            +myusage.ru_stime.tv_usec/1000000.0;
    systime += (double)childusage.ru_stime.tv_sec
            +childusage.ru_stime.tv_usec/1000000.0;

    printf("usertime: %g, systime: %g\n",usertime,systime);
    // %g:使用浮点形式(%f)和科学记数形式(%e)中较短的那个输出，并省略多余的0
}
