
#include<iostream>
#include<string>
#include<map>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/epoll.h>
#include<string.h>
#include<error.h>
#include<errno.h>
#include<stdlib.h>
#include<sys/resource.h>
#include<pthread.h>
#include<time.h>
#include<stdio.h>

using namespace std;

#define LISTENQ 64
#define MAX_EVENTS 10000
#define BUFSIZE 8192

static unsigned long countacp=0;
//static unsigned long countin=0;
//static unsigned long countout=0;
//static unsigned long counttimeout=0;

struct client_t{
        int fd;
        int stat;
        string ip;
        string read_buf;
        string write_buf;
        time_t last_alive_time;
        int is_timeout;
        client_t(){
                is_timeout = 0;
                stat = 0;
        }
};

map<int, struct client_t *> fds;
pthread_mutex_t fds_mutex = PTHREAD_MUTEX_INITIALIZER;

static string int2str(int i){
        char buf[32];
        memset(buf, 0, 32);
        sprintf(buf, "%d", i);
        return string(buf);
}

//initialize server socket
//正常返回fd,否则返回-1
static int
init_servsock()
{
        int fd;
        struct sockaddr_in serveraddr;
        //socket
        if((fd=socket(AF_INET, SOCK_STREAM, 0)) < 0){
                perror("socket");
                return -1;
        }

        //可重用
        socklen_t reuse=1;
        if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))<0){ //否则程序挂掉马上重启,会报端口不可用
                perror("setsockopt()");
        }

        serveraddr.sin_family = AF_INET;
        serveraddr.sin_port = htons(80);
        serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
        bzero(&(serveraddr.sin_zero), 8);
        //bind
        if(bind(fd,(struct sockaddr *)&serveraddr, sizeof(sockaddr)) < 0){
                perror("bind");
                return -1;
        }
        //listen
        if(listen(fd, LISTENQ)){
                perror("listen");
                return -1;
        }

        return fd;
}

static ssize_t
writen(int fd, const char *vptr, size_t n)
{
        size_t nleft;
        ssize_t nwritten;
        const char *ptr;

        ptr = vptr;
        nleft = n;
        while(nleft > 0){
                if((nwritten = write(fd, ptr, nleft)) <= 0)
                        return(nwritten); /* error */

                nleft -= nwritten;
                ptr += nwritten;
        }
        return(n);
}

static ssize_t
readn(int fd, char *vptr, size_t n)
{
        size_t nleft;
        ssize_t nread;
        char *ptr;

        ptr = vptr;
        nleft = n;
        while(nleft > 0){
                if((nread = read(fd, ptr, nleft) < 0))
                        return(nread);        /* error, return < 0 */
                else if(nread ==0 ){
                        break;                /* EOF */
                }

                nleft -= nread;
                ptr += nread;
        }
        return(n - nleft);                    /* return >= 0 */
}

static int
set_fl(int fd, int flags)
{
        int val;

        if((val = fcntl(fd,F_GETFL,0)) < 0){
                printf("fcntl F_GETFL error\n");
                return -1;
        }

        val |= flags;

        if(fcntl(fd,F_SETFL,val) < 0){
                printf("fcntl F_SETFL error\n");
                return -1;
        }

        return 0;
}

//超时检测线程函数
static void *
check_time_out(void * arg){
        int epollfd=*((int *)arg);
        printf("epollfd: %d\n",epollfd);
        while(1){
                map<int,client_t *>::iterator it = fds.begin();
                time_t tm;
                time_t run_tm;
                time(&tm);
                for(; it!=fds.end(); ++it){
                        if(it->second != NULL){
                                run_tm = tm - it->second->last_alive_time;
                                if(run_tm >= 30){
                                        pthread_mutex_lock(&fds_mutex);
                                        printf("fd: %d timeout!\n",it->second->fd);
                                        fflush(NULL);
                                        //counttimeout++;
                                        epoll_ctl(epollfd, EPOLL_CTL_DEL, it->second->fd, NULL);
                                        shutdown(it->second->fd, SHUT_RDWR);
                                        close(it->second->fd);
                                        delete it->second;
                                        it->second = NULL;
                                        pthread_mutex_unlock(&fds_mutex);
                                }
                        }
                }
                sleep(2);
        }
        pthread_exit(NULL);
}

int
main()
{
        struct epoll_event ev, events[MAX_EVENTS];
        int listen_sock;
        int epollfd;
        int nfds;
        int err;

        //更改系统打开文件限制
        struct rlimit limit;
        limit.rlim_cur = 8192;//getdtablesize();//=1024
        limit.rlim_max = limit.rlim_cur;
        if(setrlimit(RLIMIT_NOFILE, &limit) < 0){
                perror("setrlimit");
                exit(1);
        }


        epollfd = epoll_create(512);
        if(epollfd == -1){
                perror("epoll_create");
                exit(1);
        }

        //创建超时检测线程
        pthread_t pid;
        err = pthread_create(&pid, NULL, check_time_out, (void *)&epollfd);
        if(err !=0 ){
                printf("创建超时检测线程失败！\n");
                exit(1);
        }

        if((listen_sock = init_servsock())<0){
                exit(1);
        }

        //设置为非阻塞
        if(set_fl(listen_sock,O_NONBLOCK) < 0){
                exit(1);
        }

        //忽略信号
        signal(SIGPIPE,SIG_IGN);

        //daemon
        //daemon(0,0);

        ev.events = EPOLLIN;
        ev.data.fd = listen_sock;
        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1){
                perror("epoll_ctl:listen_sock");
                exit(1);
        }



        while(1){
                //printf("accept:%u\n",countacp);
                //printf("in:%u\n", countin);
                //printf("out:%u,\n", countout);
                //printf("timeout:%u\n", counttimeout);
                //printf("\n");
                //fflush(NULL);
                nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
                if(nfds == -1){
                        perror("epoll_wait");
                        exit(1);
                }

                int n;
                for(n = 0; n < nfds; ++n){
                        if(events[n].data.fd == listen_sock){
                                struct sockaddr_in addr_in;
                                size_t addrlen = sizeof(struct sockaddr_in);
                                pthread_mutex_lock(&fds_mutex);
                                int conn_sock = accept(listen_sock, (struct sockaddr *)&addr_in, &addrlen);
                                time_t tm=time(NULL);
                                char tbuf[32];
                                memset(tbuf, 0, 32);
                                strftime(tbuf,32,"%Y-%m-%d %H:%M:%S", localtime(&tm));
                                printf("%s\n", tbuf);
                                fflush(NULL);
                                if(conn_sock == -1){
                                        perror("accept");
                                        continue;
                                }

                                if(set_fl(conn_sock, O_NONBLOCK) < 0){
                                        close(conn_sock);
                                        continue;
                                }
                                ev.events = EPOLLIN | EPOLLET;
                                ev.data.fd = conn_sock;
                                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1){
                                        perror("epoll_ctl:conn_sock");
                                        close(conn_sock);
                                        continue;
                                }

                                countacp++;
                                client_t *client_ptr = new client_t;
                                fds[conn_sock] = client_ptr;
                                fds[conn_sock]->fd = conn_sock;
                                fds[conn_sock]->ip = string(inet_ntoa(addr_in.sin_addr));
                                fds[conn_sock]->last_alive_time = time(NULL);
                                pthread_mutex_unlock(&fds_mutex);
                        }else if(events[n].events & EPOLLIN){
                                int fd = events[n].data.fd;
                                int n;
                                char buf[BUFSIZE];
                                memset(buf, 0, BUFSIZE);

                                n = readn(fd, buf, BUFSIZE);
                                pthread_mutex_lock(&fds_mutex);
                                fds[fd]->read_buf.append(buf,BUFSIZE);
                                cout<<fds[fd]->read_buf<<endl<<endl;
                                if(n >= 0){
                                        //if(fds[fd]->read_buf.find("\r\n\r\n") != string::npos){                /* 检测请求头是否结束 */
                                                //countin++;
                                                ev.events = EPOLLOUT | EPOLLET;
                                                ev.data.fd = fd;
                                                epoll_ctl(epollfd, EPOLL_CTL_MOD ,fd ,&ev);
                                        //}
                                }else{//出错
                                        if(errno == EAGAIN){
                                                continue;
                                        }
                                        shutdown(fds[fd]->fd, SHUT_RDWR);
                                        close(fds[fd]->fd);
                                        delete fds[fd];
                                        fds[fd] = NULL;
                                }
                                fds[fd]->last_alive_time=time(NULL);

                                pthread_mutex_unlock(&fds_mutex);

                        }else if(events[n].events & EPOLLOUT){
                                int fd = events[n].data.fd;
                                pthread_mutex_lock(&fds_mutex);
                                string wb = "<html><head><title>hello world</title></head><body>hello world!!!</body></html>";
                                fds[fd]->write_buf = "HTTP/1.1 200 Ok\r\n";
                                fds[fd]->write_buf += "Content-Type: text/html\r\n";
                                fds[fd]->write_buf += "Content-Length: "+int2str(wb.length())+"\r\n";
                                fds[fd]->write_buf += "\r\n";
                                fds[fd]->write_buf +=wb;
                                int n = writen(fd, fds[fd]->write_buf.data(), fds[fd]->write_buf.length());

                                fds[fd]->last_alive_time=time(NULL);
                                if(n == fds[fd]->write_buf.length() || n < 0){
                                        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
                                        shutdown(fd,SHUT_RDWR);
                                        close(fd);
                                        delete fds[fd];
                                        fds[fd] = NULL;
                                        //countout++;
                                }
                                pthread_mutex_unlock(&fds_mutex);
                        }
                }
        }

        close(listen_sock);
        close(epollfd);

        return 0;
}
