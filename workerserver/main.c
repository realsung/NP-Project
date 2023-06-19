#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>

#define BUF_SIZE 1024
#define MAX_SOCKET 24
#define MAX_QUEUE 24

struct Job
{
    uint32_t jobid;
    char challenge[128];
    uint8_t diffi;
};


struct Job roundQueue[MAX_QUEUE];
int rqhead=0;
int rqtail=0;

int main(int argc, char* argv[]){
    if(argc<2)exit(1);

    char buf[BUF_SIZE]={0,};
    
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((errno = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errno));
        exit(1);
    }

    struct sockaddr_in addr, clientaddr;
    struct epoll_event ev, events[MAX_SOCKET];
    int listenfd,epollfd;

    if((epollfd = epoll_create(100)) == -1)
        return 1;
 
    int addrlen = sizeof(addr);
    if((listenfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
        return 1;
    if(bind(listenfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        return 1;
 
    listen(listenfd, 16);
 
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);     
    int workingpid=-2;

    while(1)
    {
        int eventn = epoll_wait(epollfd, events, MAX_SOCKET, 100); 
        if(eventn == -1)return 1;
        for(int i = 0; i < eventn ; i++)
        {
            if(events[i].data.fd == listenfd)
            {
                int clilen = sizeof(struct sockaddr);
                int clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clilen);                
                ev.events = EPOLLIN; 
                epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);
            }
            else
            {
                int fd=events[i].data.fd;
                int readn = read(fd, buf, 2);
                if(readn <= 0&&buf[0]==0x55)
                {
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, events);
                    close(fd);
                }
                else{                    
                    uint8_t diff;
                    int jobid;
                    uint8_t challlen;

                    read(fd,&challlen,1);
                    read(fd,buf,challlen);
                    buf[challlen]=0;
                    read(fd,&jobid,4);
                    read(fd,&diff,1);
                    
                    execv("./bin/spawner",(char**){"./bin/spawner",buf,itoa(diff)});

                    // roundQueue[rqtail].challenge
                    // send_msg(events[i], buf);
                }
            }
        }
    }
}

