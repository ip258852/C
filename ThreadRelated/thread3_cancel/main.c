#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void    *threadFuc(void *arg);

int main()
{
    pthread_t p1;
    int s;
    void *res;

    s=pthread_create(&p1,NULL,threadFuc,NULL);
    if(s!=0){
        printf("create error\n");
        exit(1);
    }

    sleep(5);

    s=pthread_cancel(p1);
    if(s!=0){
        printf("cancle error\n");
        exit(1);
    }

    s=pthread_join(p1,&res);
    if(s!=0){
        printf("join error\n");
        exit(1);
    }

    if(res== PTHREAD_CANCELED)
        printf("canceld\n");
    else
        printf("uncanceld\n");

    return 0;
}

void    *threadFuc(void *arg){
    int i;

    printf("new start\n");
    for(i=0;;i++){
        printf("the i is %d\n",i);
        sleep(1);
    }
    return NULL;
}
