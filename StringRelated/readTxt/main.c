/*
  Author     : Kobe
  Date       : 20170719
  Description: fileRead2Char*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#define BUF_SIZE 206  // almost 256~512,

int  check_file_line(FILE *target);

void check_file_open(FILE *target);
void get_file_line_sentence(FILE *target,char **data);
void print_data(char **data,int line);

int main()
{
    FILE *target;   //u want to open
    char **data;    //the data u want to save.

    int line;       //for malloc data
    int i;

    target=fopen("paper.txt","r");

    check_file_open(target);
    line=check_file_line(target);

    data= (char**)malloc(sizeof(char*)*line);
    for(i=0;i<line;i++){
        data[i]=(char*)malloc(sizeof(char)*BUF_SIZE);
    }

    get_file_line_sentence(target,data);
    print_data(data,line);

    for(i=0;i<line;i++){
        free(data[i]);
    }
    free(data);

    return 0;
}

/*
    @summary:return how many lines in .txt.Use line to malloc char**.
    @par:[FILE:target],the file you want to check.
    @return:line
*/
int  check_file_line(FILE *target){
    char *ptr;
    char rbuf[BUF_SIZE]={""};    //rLine Buffer
    int  line_cnt=0;
    int  readSize;

    while(1){
        memset(rbuf,' ',sizeof(rbuf)); //Ini
        readSize=fread(rbuf, 1, BUF_SIZE, target); //for break loop

        ptr=strchr(rbuf,'\n');

        while(ptr != NULL){
            line_cnt++;
            ptr=strchr(ptr+1,'\n');
        }//End for while

        if(BUF_SIZE!=readSize) break;

    }//End for while

    printf("line_cnt is %d\n",line_cnt);
    fseek(target,0,SEEK_SET);   //back to file origins

    return line_cnt;
}


/*
    @summary: check file is open or fail.
    @par:[FILE:target],the file you want to check.
    @return:None
*/
void check_file_open(FILE *target){
    if(target)
        printf("The fileOpen is OK.\n");
    else{
        printf("Wrong\n");
    }//End for if-else
}

/*
    @summary:save .txt in char **data.
    @par1:[FILE:target],the file you want to check.
    @par2:[char**:data],the file data you want to save.
    @return:None.
*/
void get_file_line_sentence(FILE *target,char **data){
    char rbuf[BUF_SIZE]={""};    //rLine Buffer

    int i=0;
    int check=1; //check using which function,strcat/strcpy

    //get a buf line
    while(fgets(rbuf,BUF_SIZE, target) != NULL){
        //check if not end
        if(strlen(rbuf)==BUF_SIZE-1){
            check=0;
        }//End for if

        if(check){
            strcpy(data[i],rbuf);
        }else{ // fix too long sentence.
            strcpy(data[i],rbuf);
            while(fgets(rbuf,BUF_SIZE, target) != NULL){
                strcat(data[i],rbuf);
                if(strlen(rbuf)!=BUF_SIZE-1){
                    break;
                }//End for if
            }//End for while
        }//End for else

        check=1;
        i++;

    }//End for while
}

/*
    @summary:print char **data.
    @par1:[char**:data],the file data you want to save.
    @par2:[int:line],the file's line.
    @return:None.
*/
void print_data(char **data,int line){
    int i;
    for(i=0;i<line;i++){
        printf("%d line : %s",i+1,data[i]);
    }
}
