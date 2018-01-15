#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define COUNT 50

// once for pthread_once() ini.
static pthread_once_t once = PTHREAD_ONCE_INIT;
// key for pthread_key_create() ini.
static pthread_key_t  key ;

//package data you want to fix.
typedef struct _data{
    int cnt;
} data;

int     simple(int number);
void    *thread(void *arg);
static  void    destroy(void *data);
static  void    createKey();

int main()
{
    pthread_t p1,p2;
    int a=1,b=2;

    pthread_create(&p1,NULL,thread,(void*)(intptr_t)a);
    pthread_create(&p2,NULL,thread,(void*)(intptr_t)b);
    pthread_join(p1,NULL);
    pthread_join(p2,NULL);

    return 0;
}

//safety function
int    simple(int number){

    data *d;
    int  s;
    //create key ,only one times
    s=pthread_once(&once,createKey);
    if(s!=0)
        printf("pthread_once error\n");

    //get key and check key value?
    d=pthread_getspecific(key);
    if(d==NULL){
        //data ini
        printf("thread [%d] set specific\n",number);
        d=malloc(sizeof(data));
        if(d==NULL)
            printf("struct malloc error\n");
        d->cnt=0;

        //set TSD buffer point which device
        s=pthread_setspecific(key,d);
        if(s!=0)
            printf("set error\n");
    }

    //do something in TSD buffer.
    d->cnt++;

    return d->cnt;
}

//thread function. Just for-loop safety function
void    *thread(void *arg){
    int number=(intptr_t)arg;
    int i;
    for(i=0;i<COUNT;i++){
        printf("the %d thread is [%d]\n",number,simple(number));
        sleep(number);
    }
}

/*
    create TSD buffer door key,and then,thread can use this key to open TSD buffer.
    PS:only once create.
*/
static  void    createKey(){
    int    s;
    s=pthread_key_create(&key,destroy);
    if(s!=0)
        printf("key_create error\n");

}

//freeing  TSD buffer is useless ,after thread closed or joined.
static void    destroy(void *data){
    free(data);
}
