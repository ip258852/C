/*
    file put crash.
*/
//normal
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
//net
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
//thread
#include <pthread.h>
//chmod
#include <sys/stat.h>

#define BUFSIZE     512
#define END         "end"
#define DIR_PATH    "data"
#define SER_IP      "10.3.103.99"
#define SER_PORT    "9487"
#define LIS_PORT    "9487"

static struct addrinfo *p=NULL;
static struct addrinfo *sp=NULL;

//Net    Related
int     sockServerCreate(int *sock,char *port);
int     sockClientCreate(int *sock,struct addrinfo **server,char *ip,char *port);
//String Related
void    get_token(char *topic,char **answer,char delimiter );

int main(int argc,char* argv[])
{

//Normal Par
    int         numByte,i=0,number=2;
//String Par
    char        buf[BUFSIZE]={0};
    char        **answer=(char **) malloc(sizeof(char*) * number);
    for(i=0;i<number;i++){
        answer[i]=(char *) malloc(sizeof(char) * 25);
    }
//Net Par
    int         sock,ssock;
    struct sockaddr_in client;
    socklen_t   reclen=sizeof(client);
//File par
    FILE        *threadFile;

    //UDP Ser Cli Open
    sockServerCreate(&sock,LIS_PORT);
    sockClientCreate(&ssock,&sp,SER_IP,SER_PORT);
    printf("Wait data name and modType\n");

    //Rec file data par
    while(1){
        recvfrom(sock,buf,BUFSIZE,0,(struct sockaddr *)&client,&reclen);
        if(strlen(buf)>=0){
            sendto(ssock,"ok",5,0,sp->ai_addr,sp->ai_addrlen);
            break ;
        }
    }

    //Token file data.
    get_token(buf,answer,';');

    printf("Start acept %s file,mod is %s \n",answer[0],answer[1]);

    //file open
    threadFile=fopen(answer[0],"wb");
    if(threadFile)
        printf("The [%s] file open\n",answer[0]);
    else{
        printf("File open error\n");
        exit(1);
    }

    //Star rec file
    memset(buf,'\0',BUFSIZE);
    while(1){
        numByte=recvfrom(sock,buf,BUFSIZE,0,(struct sockaddr *)&client,&reclen);
        if(!strcmp(END,buf))
            break;

        fwrite(buf,sizeof(char),numByte,threadFile);

        memset(buf,'\0',BUFSIZE);
    }
    printf("File rec end.\n");


    //mod file
    chmod(answer[0],atoi(answer[1]));

    //Close
    for(i=0;i<number;i++){
        free(answer[i]);
    }
    free(answer);
    fclose(threadFile);
    close(sock);

    return 0;
}

void    get_token(char *topic,char **answer,char delimiter){
  char *topic_copy=(char *) malloc(sizeof(char)*strlen(topic)+1);
  char *temp;
  int  i=0;
   if( topic_copy == NULL ) {

    return ;
  }

  strcpy(topic_copy,topic);
  while(1){
    temp=strchr(topic_copy,delimiter);
    if(temp==NULL) {
       strcpy(answer[i],topic_copy);
       break;
    }
    temp[0]='\0';
    strcpy(answer[i],topic_copy);
    strcpy(topic_copy,temp+1);
    i++;
  }

  free(topic_copy);
}

int     sockServerCreate(int *sock,char *port){

    int status;
    int yes=1;
    struct addrinfo hint,*infor;

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

int     sockClientCreate(int *sock,struct addrinfo **server,char *ip,char *port){

    int status;
    int yes=1;
    struct addrinfo hint,*infor;
    struct addrinfo *p1=malloc(sizeof(*p1));

    memset(&hint,0,sizeof(hint));

    hint.ai_family=AF_UNSPEC;
    hint.ai_socktype=SOCK_DGRAM;

    if((status=getaddrinfo(ip,port,&hint,&infor))!=0){
        printf("getaddrinfo error\n");
        return 1;
    }

    //check addrinfotmation for "One" socket use,then,create socket() & bind()
    for(p1=infor;p1!=NULL;p1=p1->ai_next){
        if (p1->ai_family == AF_INET){
            if((*sock=socket(p1->ai_family,p1->ai_socktype,p1->ai_protocol))==-1){
                printf("socket error\n");
                continue ;
            }

            if (setsockopt(*sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
                printf("setsockopt error\n");
                return 1;
            }
        }

        break;
    }

    if(p1==NULL){
        printf("addrinfot error\n");
        return 1;
    }

    *server=p1;

    freeaddrinfo(infor);

    return 0;
}
