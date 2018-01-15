#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
static int glob =0;

void    *threadFuc(void *arg);
void    *cleanupHandler(void *arg);

int main(int args,char *argv[])
{
    pthread_t p1;
    void *res;
    int s;

    s=pthread_create(&p1,NULL,threadFuc,NULL);
    if(s!=0){
        printf("create error\n");
        exit(1);
    }

    sleep(2);

    if(args==1){
        printf("main: about to cancel thread\n");
        s=pthread_cancel(p1);
        if(s!=0){
            printf("create error\n");
            exit(1);
        }
    }else{
        printf("main: about to signal condition variable\n");
        glob=1;
        s=pthread_cond_signal(&cond);
        if(s!=0){
            printf("cond_signal error\n");
            exit(1);
        }
    }

    s=pthread_join(p1,&res);
    if(s!=0){
        printf("join error\n");
        exit(1);
    }

    if(res==PTHREAD_CANCELED)
        printf("main:  thread was cancel\n");
    else
        printf("main:  thread terminated normally\n");

    return 0;
}

void    *threadFuc(void *arg){
    int s;
    void *buf=NULL;

    buf=malloc(0x10000);
    printf("thread: allocated memory at %p\n",buf);

    s=pthread_mutex_lock(&mutex);
    if(s!=0){
        printf("mutex create error\n");
        exit(1);
    }

    pthread_cleanup_push(cleanupHandler,buf);

    while(glob ==0){
        s=pthread_cond_wait(&cond,&mutex);
        if(s!=0){
            printf("cond wait error\n");
            exit(1);
        }
    }

    printf("thread: condition loop completed\n");
    pthread_cleanup_pop(1);
    return NULL;
}

void    *cleanupHandler(void *arg){
    int s;

    printf("clean up handler at %p\n",arg);
    free(arg);

    printf("cleanup: unlocking mutex\n");
    s=pthread_mutex_unlock(&mutex);
    if(s!=0){
        printf("mutex_unlock error\n");
        exit(1);
    }
}
