#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DIM 50

void get_token(char *topic,char **answer,char delimiter );
void show_token(char **answer,int number);

int main(){

  int number =4,i;
  char *topic="SSS;ssss;ss;dCCC,ssss,dddd,cccc";
  char **answer=(char **) malloc(sizeof(char*) * number);
  for(i=0;i<number;i++){
    answer[i]=(char *) malloc(sizeof(char) * 25);
  }

  get_token(topic,answer,',');
  show_token(answer,number);

  for(i=0;i<number;i++){
    free(answer[i]);
  }
  free(answer);
  return 0;
}


void get_token(char *topic,char **answer,char delimiter){
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
    printf("%p\n",topic_copy);
    i++;
  }

  free(topic_copy);
}
/*
void get_token(char *topic,int number,char **answer,char delimiter ){
  char topic_copy[DIM];
  char *temp;

  int  i=0;

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

}
*/
void show_token(char **answer,int number){
   int i;
   for(i=0;i<number;i++){
    printf("%s\n",answer[i]);
   }
}
