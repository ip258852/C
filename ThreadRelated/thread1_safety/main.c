#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/msg.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>

#define BUF_SIZE 512

static pthread_once_t once=PTHREAD_ONCE_INIT;
static pthread_key_t  key;

char    *giveAndGet(int number);
void    *threadFuc(void *arg);
void    destructor(void *buf);
void    createKey(void);


int main(int argc,char* argv[])
{
    pthread_t t;
    char *str;
    int s;
    int number=1;

    str=giveAndGet(3);

    s=pthread_create(&t,NULL,threadFuc,(void *)(intptr_t)number);
    if(s!=0){
        printf("create error\n");
        exit(1);
    }

    s=pthread_join(t,NULL);
    if(s!=0){
        printf("join error\n");
        exit(1);
    }

    printf("Main buf is [%s]\n",str);

    return 0;
}

char    *giveAndGet(int number){
    int s;
    char *buf;

    s=pthread_once(&once,createKey);
    if(s!=0){
        printf("once error\n");
        exit(1);
    }

    buf=pthread_getspecific(key);
    if(buf==NULL){
        buf=malloc(BUF_SIZE);
        if(buf==NULL)
            printf("buf error\n");

        s=pthread_setspecific(key,buf);
        if(s!=0)
            printf("setspecific error");

    }

    switch(number){
        case 0:
            sprintf(buf,"you give me 0");
            break;
        case 1:
            sprintf(buf,"you give me 1");
            break;
        case 2:
            sprintf(buf,"you give me 2");
            break;
        default:
            sprintf(buf,"I dont know");
            break;
    }
    return buf;
}

void    *threadFuc(void *arg){
    int number=(intptr_t)arg;
    char *str=giveAndGet(number);
    printf("thread buf is [%s]\n",str);
    return NULL;
}

void    destructor(void *buf){
    free(buf);
}

void    createKey(void){
    int s;

    s=pthread_key_create(&key,destructor);
    if(s!=0){
        printf("key_create error");
    }

}
