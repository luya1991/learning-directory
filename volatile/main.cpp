#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<time.h>
#include<sys/time.h>

volatile int t;
/*
    2014-5-25
    Note:
    未添加volatile关键字以前的运行结果是，t都减小到-8了，程序还没停止。
    添加了volatile关键字以后，程序在t=0的时候就自动退出了。

    说明：
    volatile表示该变量是“易变的，易失性的，易挥发的”
    t除了内存中存储了值，还在寄存器（或缓存）里存储了值，

        *volatile可以避免编译器的自动优化。

    编译器自动优化导致编译器读取寄存器中的那个t值，那个t值就是10，
    而信号处理函数修改的t值是那个内存中的值，
    所以，
    未添加volatile时，程序会是一个死循环，
    而添加volatile以后，程序每次需要读取t值时，都会小心地按内存指针重新读取，
    因此程序将在t=0时正确退出。
*/

void alarm_handler(int signo)
{
    printf("get a alarm signal\n");
    t--;
    printf("t=%d\n",t);
}

int main()
{
    signal(SIGALRM,alarm_handler);

    itimerval it_new,it_old;

    it_new.it_interval.tv_sec = 1;
    it_new.it_interval.tv_usec = 0;
    it_new.it_value.tv_sec = 1;
    it_new.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL,&it_new,&it_old);
    t=10;
    while(t!=0)
        ;
    return 0;
}
