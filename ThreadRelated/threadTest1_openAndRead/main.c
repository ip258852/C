#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
//Net Related
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
//Dir Related
#include <dirent.h>
//Mod Related
#include <sys/types.h>
#include <sys/stat.h>

#define BUF_SIZE    512
#define END         "end"
#define DIR_PATH    "data"
#define SER_IP      "10.3.103.88"
#define SER_PORT    "9487"
#define LIS_PORT    "9487"
#define DATAMTYPE   S_IXUSR|S_IRUSR|S_IWUSR

typedef struct node node;
typedef struct queue queue;
typedef struct dirent dirent;
typedef struct fileTarget fileTarget;

struct node {
    char name[BUF_SIZE];
    int  rNuber;
    node *next;
};

struct queue {
    node *head;
    node *tail;
};

struct fileTarget{
    int     number;
    char    fileName[25];
};

//UDP Glo par
static struct addrinfo *p=NULL;
static struct addrinfo *sp=NULL;
//Queue Glo par
static queue *q;
static int sock;
static int ssock;

//Queue Related
int     push(queue *q,char *name,int number);
node    *pop(queue *q );

//Thread Related
void    *thread(void *arg);
//File && dir Related
int     getDirFileNumber();
int     fileBitOpenCheck(FILE **file,char *name,char *mod);
//Normal Related
int     fixError(char *msg);
//Net Related
int     sockClientCreate(int *sock,struct addrinfo **server,char *ip,char *port);
int     sockServerCreate(int *sock,char *port);

int main()
{
//thread set
    pthread_t   p1;

//FILE I/O set
    fileTarget  ft;
    FILE        *mainFile;
    DIR         *d;
    dirent      *dirent;
    int         fileNumber=getDirFileNumber();

//Normal set
    int         s,number,i;
    char        buf[BUF_SIZE];
    char        command[50];
    char        **fileName=(char **)malloc(sizeof(char*)*fileNumber);
    for(i=0;i<fileNumber;i++){
        fileName[i]=(char*)malloc(sizeof(char)*25);
    }

//Queue set
    node        *rbuf=malloc(sizeof(node));
    q=malloc(sizeof(queue));

    //FILE I/O ini and choose
    d=opendir(DIR_PATH);
    if(d==NULL){
        printf("open dir error\n");
        exit(1);
    }

        //show file
    printf("Print file in data dir \n\n");
    for(i=0;i<fileNumber;i++){
        if((dirent=readdir(d))==NULL)
            break;
        strcpy(fileName[i],dirent->d_name);
        printf("[%d] %s\t",i+1,dirent->d_name);
    }
    printf("\n");

        //choose file
    printf("Choose file you want:");
    while(1){
        scanf("\n%d",&i);
        if( i>=1 && i <=  fileNumber ){
            printf("Number is Right\n");
            sprintf(ft.fileName,"%s/%s",DIR_PATH,fileName[(i-1)]);
            ft.number=i-1;
            break;
        }
        printf("Wrong Number\n");
        printf("Choose file again:");
    }

    //fileOpen && push file name
    fileBitOpenCheck(&mainFile,ft.fileName,"rb");
    sprintf(command,"%s;%d",ft.fileName,DATAMTYPE);
    push(q,command,strlen(command));

    //create UDP
    sockServerCreate(&sock,LIS_PORT);
    sockClientCreate(&ssock,&sp,SER_IP,SER_PORT);

    //thread create
    s=pthread_create(&p1,NULL,thread,NULL);
    if(s!=0)
        fixError("thread create");

    //thread exchange data by quene
    while(1){
        number=fread(buf,sizeof(char),BUF_SIZE,mainFile);
        push(q,buf,number);
        if(feof(mainFile)){
            push(q,END,strlen(END));
            break;
        }
    }

    //join thread
    s=pthread_join(p1,NULL);
    if(s!=0)
        fixError("thread join");

    //close && free Related
    for(i=0;i<fileNumber;i++){
        free(fileName[i]);
    }
    free(fileName);
    free(rbuf);
    free(q);
    close(sock);
    close(ssock);

    return 0;
}

int     fileBitOpenCheck(FILE **file,char *name,char *mod){
    file[0]=fopen(name,mod);
    if(file[0])
        printf("The [%s] file open.\n\n",name);
    else{
        fixError("file open");
        exit(1);
    }
    return 0;
}

void    *thread(void *arg){
    node        *buf=malloc(sizeof(node));
    char        rbuf[BUF_SIZE];
    struct      sockaddr_in client;
    socklen_t   reclen=sizeof(client);


    buf=pop(q);
    sendto(ssock,buf->name,buf->rNuber,0,sp->ai_addr,sp->ai_addrlen);
    printf("Send file name and mod.\n");

    while(1){
        printf("Wait ack...\n");
        memset(rbuf,'\0',BUF_SIZE);
        recvfrom(sock,rbuf,BUF_SIZE,0,(struct sockaddr *)&client,&reclen);

        if(!strcmp("ok",rbuf)){
            printf("Send data start \n");
            break;
        }
        else
            printf("Error ack\n");
    }

    while(1){
        //dequeue and check file end
        buf=pop(q);
        if(!strcmp(buf->name,END)){
            sendto(ssock,buf->name,buf->rNuber,0,sp->ai_addr,sp->ai_addrlen);
            free(buf);
            break ;
        }
        sendto(ssock,buf->name,buf->rNuber,0,sp->ai_addr,sp->ai_addrlen);
    }
    printf("Send Data End\n");
    return NULL;
}

int     fixError(char *msg){
    printf("%s error\n",msg);
    return -1;
}

int     push(queue *q,char *name,int number){

    node *temp=malloc(sizeof(node));
    if(temp==NULL)
        return fixError("push temp node create");

    memcpy(temp->name,name,BUF_SIZE);
    temp->rNuber=number;
    temp->next=NULL;

    if(q->tail!=NULL)
        q->tail->next=temp;

    q->tail=temp;
    if(q->head==NULL){
        q->head=temp;
    }

    return 0;
}

node    *pop(queue *q){
    node *temp=malloc(sizeof(node));

    if(q->head==NULL){
        q->tail=NULL;
        strcpy(temp->name,END);
        return temp;
    }

    temp=q->head;
    q->head=q->head->next;
/*
    temp->next=NULL;
    free(temp);
*/
    return temp;
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

int     getDirFileNumber(){
    int i=0;
    DIR         *d;
    dirent      *dirent;

    d=opendir(DIR_PATH);
    if(d==NULL){
        printf("open dir error\n");
        exit(1);
    }

    while((dirent=readdir(d))!=NULL)
        i++;

    return i;
}
