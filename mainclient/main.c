#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <time.h>

#include <string.h>
#include <stdio.h>

#define EPOLL_SIZE 20
#define BUF_SIZE 1024
#define MAX_SOCKET 24

int socketFD[MAX_SOCKET];
int socketCnt=0;
int roundrobin=0;

struct addrinfo hints;
void connectWorkers(char *buf){
    struct addrinfo *servinfo, *p;
    char* host = strtok(buf,":");
    char* port = strtok(NULL,"\n");
        
    if ((errno = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(errno));
        exit(1);
    }

    for(p = servinfo;p != NULL; p = p->ai_next) {
        void *addr;
        char *ipver;

        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
        int sockfd=-1;
        if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1){
            fprintf(stderr, "socket error: %s\n", strerror(errno));
            exit(1);
        }
        
        if(connect(sockfd,p->ai_addr, p->ai_addrlen)){
            fprintf(stderr, "connect error: %s\n", strerror(errno));
            exit(1);
        }else{
            socketFD[socketCnt++]=sockfd;
            break;
        }
    }
    
    freeaddrinfo(servinfo);
}

void sendChallenge(char *buf){
    int fd=socketFD[(roundrobin++)%socketCnt];
    char *chal=strtok(buf," ");
    uint8_t diff=atoi(strtok(buf,"\n"));
    int jobid=(time(NULL)<<12)+rand();
    uint8_t challlen=strlen(chal);
    write(fd,"\x55\x01",2);

    write(fd,&challlen,1);
    write(fd,chal,challlen);
    write(fd,&jobid,4);
    write(fd,&diff,1);
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    
    int epollfd;
    struct epoll_event ev, events[MAX_SOCKET];
    if((epollfd = epoll_create(100)) == -1)
        return 1;

    if(argc<3)return -1;
    FILE *fp=fopen(argv[1],"r");
    char buf[BUF_SIZE]={0,};
    while(fgets(buf,BUF_SIZE-1,fp)!= NULL){
        connectWorkers(buf);
        ev.events = EPOLLIN; 
        ev.data.u32 = socketCnt-1;
        epoll_ctl(epollfd, EPOLL_CTL_ADD, socketFD[socketCnt-1], &ev);
    } 
    fclose(fp);

    fp=fopen(argv[2],"r");
    while(fgets(buf,BUF_SIZE-1,fp)!= NULL){
        sendChallenge(buf);
    }
    fclose(fp);
    
    for(int i=0;i<socketCnt;i++){
        close(socketFD[i]);
    }
    // gcc mainclient/main.c -o bin/mainclient

}
