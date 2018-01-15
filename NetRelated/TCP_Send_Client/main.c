#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <poll.h>

#define BUFSIZE     512


int sockClientCreate(int *sock,char* IP,char *port);
int sockClientSend(int *sock,char* IP,char *port);
void periodSend(int *sock);


int main(int argc, char *argv[])
{
    int  sock,status,i=1;

    if(argc<3){
        printf("less par.");
        return 0;
    }

    while(1){
        status=sockClientCreate(&sock,argv[1],argv[2]);
        if(status==-1){
            printf("retry to connection %d\n",i);
            i++;
        }else
            break;
        sleep(1);
    }

    printf("connect OK\n");
    sockClientSend(&sock,argv[1],argv[2]);

    return 0;
}

int sockClientCreate(int *sock,char* IP,char *port){
    int status;
    int setSocket_par=1;
    struct addrinfo hint,*p,*infor;

    memset(&hint,0,sizeof(hint));
    hint.ai_family=AF_UNSPEC;
    hint.ai_socktype=SOCK_STREAM;

    if((status=getaddrinfo(IP,port,&hint,&infor))!=0){
        printf("getaddrinfo error\n");
        return -1;
    }

    //check addrinfotmation for "One" socket use,then,create socket() & bind()
    for(p=infor;p!=NULL;p=p->ai_next){

        if((*sock=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
            printf("socket error\n");
            continue ;
        }

        if (setsockopt(*sock,SOL_SOCKET,SO_REUSEADDR,&setSocket_par,sizeof(int)) == -1) {
            printf("setsockopt error\n");
            return -1;
        }
        printf("try to connect\n");
        if(connect(*sock,p->ai_addr,p->ai_addrlen)==-1){
            printf("connect error\n");
            continue;
        }

        break;
    }


    if(p==NULL){
        //printf("addrinfot error\n");
        return -1;
    }

    freeaddrinfo(infor);

    return 0;
}

int sockClientSend(int *sock,char* IP,char *port){

    char rbuf[5];
    unsigned int diff;
    int status,i=0,cnt=0;
    struct timeval before,after;
    struct pollfd user[1];
    user[0].fd=*sock;
    user[0].events=POLL_IN;

    gettimeofday(&before,NULL);

    while(1){
        gettimeofday(&after,NULL);
        diff=after.tv_sec-before.tv_sec;
        if(diff>=10){
            send(*sock,"ok",2,0);
            poll(user,1,10*1000);
            if(!(user[0].revents&POLL_IN)){
                printf("Server error\n");
                close(*sock);
                while(1){
                    status=sockClientCreate(sock,IP,port);
                    if(status==-1){
                        printf("retry to connection %d\n",i);
                        i++;
                    }else
                        break;
                    sleep(1);
                }
            }else{
                memset(rbuf,'\0',sizeof rbuf);
                recv(user[0].fd,rbuf,sizeof rbuf,0);
                printf("rbuf is %s\n",rbuf);
            }

            gettimeofday(&after,NULL);
            before=after;
        }

        send(*sock,"HI",2,0);
        printf("cnt is %d\n",cnt);
        cnt++;

        sleep(1);
    }

    return 0;
}

