#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 512

int     writefile(char *buf,int num,FILE *file);

int main()
{
    FILE *file;
    FILE *wfile;
    char buf[BUFSIZE];
    int  number,i;

    file=fopen("TCP_Rec_Server","rb");
    if(file)
        printf("open\n");
    else
        printf("open error\n");
    wfile=fopen("wTCP_Rec_Server","wb");
    if(file)
        printf("open\n");
    else
        printf("open error\n");

    i=0;
    while(!feof(file)){
        number=fread(buf,sizeof(char),BUFSIZE,file);
        printf("%d number is [%d]\n",i,number);
        printf("%s\n",buf);
        writefile(buf,number,wfile);
        memset(buf,'\0',BUFSIZE);
        i++;
    }

    fclose(file);
    fclose(wfile);

    return 0;
}

int     writefile(char *buf,int num,FILE *file){

    fwrite(buf,sizeof(char),num,file);

    return 1;
}
