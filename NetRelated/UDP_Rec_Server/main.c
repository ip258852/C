/*
    file put crash.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFSIZE 512

int sockServerCreate(int *sock,char *port);

int main(int argc,char* argv[])
{
    int status,sock;
    char buf[BUFSIZE]={0};
    socklen_t reclen;
    struct sockaddr_in client;
    FILE *threadFile;

    threadFile=fopen("Data21","w");
    if(threadFile)
        printf("the main file open\n");
    else
        return 0;

    sockServerCreate(&sock,"9487");
    reclen=sizeof(client);

    while(1){
        fflush(stdin);
        status=recvfrom(sock,buf,BUFSIZE,0,(struct sockaddr *)&client,&reclen);
        printf("%s",buf);
        memset(buf,'\0',BUFSIZE);

    }

    return 0;
}

int sockServerCreate(int *sock,char *port){

    int status;
    int yes=1;
    struct addrinfo hint,*p,*infor;

    memset(&hint,0,sizeof(hint));
    hint.ai_flags=AI_PASSIVE;
    hint.ai_family=AF_UNSPEC;
    hint.ai_socktype=SOCK_DGRAM;

    if((status=getaddrinfo(NULL,port,&hint,&infor))!=0){
        printf("getaddrinfo error\n");
        return 1;
    }

    //check addrinfotmation for "One" socket use,then,create socket() & bind()
    for(p=infor;p!=NULL;p=p->ai_next){
        if((*sock=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
            printf("socket error\n");
            continue ;
        }

        if (setsockopt(*sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
            printf("setsockopt error\n");
            return 1;
        }

        if(bind(*sock,p->ai_addr,p->ai_addrlen)==-1){
            printf("bind error\n");
            continue;
        }
        break;
    }

    if(p==NULL){
        printf("addrinfot error\n");
        return 1;
    }


    freeaddrinfo(infor);

    return 0;
}
