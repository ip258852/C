#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#define BUFSIZE 512
#define USER    25
#define LOG     15

int sockServerCreate(int *sock,char *port);
int sockServerProcess(int *sock);

int main(int number,char* par[])
{

    int  sock;

    if(number<2){
        printf("less par");
        return 0;
    }

    sockServerCreate(&sock,par[1]);

    sockServerProcess(&sock);

    close(sock);

    return 0;
}

int sockServerCreate(int *sock,char *port){

    int status;
    int yes=1;
    struct addrinfo hint,*p,*infor;

    memset(&hint,0,sizeof(hint));
    hint.ai_flags=AI_PASSIVE;
    hint.ai_family=AF_UNSPEC;
    hint.ai_socktype=SOCK_STREAM;

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

    if(listen(*sock,LOG)==-1){
        printf("listen error\n");
        return 0;
    }

    freeaddrinfo(infor);

    return 0;
}

int sockServerProcess(int *sock){
    int i,nsock,maxP,record;
    socklen_t reclen;
    struct pollfd user[USER];
    struct sockaddr_storage *client;
    char buf[BUFSIZE]={0};

    //ini
    reclen=sizeof(client);
    for(i=0;i<USER;i++){
        if(i==0){
            user[i].fd=*sock;
            user[i].events=POLLIN;
        }else
            user[i].fd=-1;
    }

    printf("start to accept.\n\n");

    while(1){

        //I don't know up 3 sock ,user[0].fd will become 0.
        //So,Design this to rebuild user[0].
        if(user[0].fd==0){
            user[0].fd=*sock;
            user[0].events=POLLIN;
        }

        record=poll(user,maxP+1,-1);

        // try to accept client
        if((user[0].revents&POLLIN)==POLLIN){
            nsock=accept(*sock,(struct sockaddr *)&client,&reclen);
            if(nsock==-1){
                printf("accept error\n");
            }

            //add to poll
            for(i=1;i<USER;i++){
                if(user[i].fd<0){
                    printf("add sock %d\n",nsock);
                    user[i].fd=nsock;
                    user[i].events=POLLIN;
                    break;
                }
            }

            //the sock in poll need to add.
            if(i>maxP)
                maxP=i;

            //
            if(--record<=0)
                continue;
        }

        for(i=1;i<=maxP;i++){
            if(user[i].fd<0)
                continue;

            if(user[i].revents & (POLLIN|POLLERR)){
                int len=0;
                memset(buf,'\0',sizeof buf);

                len=recv(user[i].fd,buf,sizeof buf,0);
                if(len<0){
                    if(errno==ECONNRESET){
                        close(user[i].fd);
                        user[i].fd=-1;
                    }else
                        printf("user%d is error\n",i);
                }else if(len==0){
                    printf("Server get user%d close. \n",user[i].fd);
                    close(user[i].fd);
                    user[i].fd=-1;
                }else{
                    if(strcmp("ok",buf)){
                        send(user[i].fd,"ack",3,0);
                    }
                    printf("Server get user%d mes is %s \n",user[i].fd,buf);
                }

                if(--record<=0)
                    break;
            }
        }

    }
    return 0;
}
