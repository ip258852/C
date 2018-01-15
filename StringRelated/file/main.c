/*
  Author     : Kobe
  Date       : 20170531
  Description: SpellRank
*/


#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 225
#define KEYWORD  (127-32)
#define ASC_START_ELEMENT 32
#define RANK_PRINT_NUMBER 10

//Cal number of spell
int *counter(char *buf,char *spell,int *number){
    //Parameter Setting;
    int i,j;
    //int number[KEYWORD]={0};


    printf("\n");
    for(i=0;i<BUF_SIZE;i++)
        {

            for(j=0;j<KEYWORD;j++)
            {
                if(buf[i]==spell[j])
                    number[j]++;
            }//End for comparing keyword.
        }//End for comparing word in File Buffer

    return number;
}

struct rank{
   char spell;
   int  number;
};

int main()
{
    //Parameter Setting;
    FILE *paperfile;
    char buf[BUF_SIZE]={""};   //FILE Buffer
    int  number[KEYWORD]={0}; // number count
    char spell[KEYWORD];  //Keyword element
    int  i;             //For variable


    //Ini
        for(i=0;i<KEYWORD;i++)
        {
            spell[i]=ASC_START_ELEMENT+i;
        }//End for spell Ini.

    //FILE open.Is it open currectly?

        paperfile=fopen("paper.txt","r");
        check_file_open(paperfile);

    //Check keyword number.
        while(1){
            fgets(buf,BUF_SIZE,(FILE*)paperfile);
            if(feof(paperfile)) break;
            for(i=0;i<1;i++){
              number[i]=counter(buf,spell,number)[i];
            }//End for.
        }//End Check keyword number.

    //sort
        bubble_sort(spell,number);

    //print No1~No10 Number
       //use printf to print
        printf("This is spellRank printf\n");
        print_spellNumber(spell,number,RANK_PRINT_NUMBER);

        printf("\n");
       //use fwrite to print
        writeRank(RANK_PRINT_NUMBER,number,spell);
        printf("This is spellRank fread\n\n");
        readRank(RANK_PRINT_NUMBER);

    //End..
        fclose(paperfile);
    return 9487;

}

void writeRank(int printNumber,int *number,char *spell){
  //Parameter Setting;
  FILE *f;
  int i;                        //For variable
  struct rank RANK={' ',0};     //ini fwrite struct element

  f=fopen("a.txt","wb");            //open mode is wirte and binary

  // saving rank to file:a
  for(i=0;i<printNumber;i++){
    RANK.number=number[i];
    RANK.spell=spell[i];
    fwrite(&RANK,sizeof(struct rank),1,f);
  }// End saving rank to file:a

  //end ......
  fclose(f);
}

void readRank(int printNumber){
  //Parameter Setting;
  FILE *f;
  int i;                    //For variable
  struct rank RANK={' ',0}; //ini fwrite struct element

  f=fopen("a.txt","rb");        //open mode is read and binary

  // read rank from file:a
  for(i=0;i<printNumber;i++){
    if(i==5) printf("\n");  //type setting

    fread(&RANK,sizeof(struct rank),1,f);
    printf("%c:%d ",RANK.spell,RANK.number);
  }// End read rank from file:a
  fclose(f);
}

void bubble_sort(char *spell,int *number){
    int i,j,temp;
    char temp_c;
    temp_c='1';
    temp=0;
    for(i=0;i<KEYWORD;i++){
       for(j=0;j<KEYWORD;j++){
           if(number[i]>number[j]){
            temp=number[j];
            number[j]=number[i];
            number[i]=temp;
            temp_c=spell[j];
            spell[j]=spell[i];
            spell[i]=temp_c;
           }
       }
    }
}

void print_spellNumber(char *spell,int *number,int printNumber){
    //Parameter Setting;
    int j;   //For variable

    for(j=0;j<printNumber;j++)
        {
            if(j%5==0)
               printf("\n"); //type setting
            printf("%c:%d ",spell[j],number[j]);
        }//End for show count
        printf("\n"); //Separated
}

void check_file_open(FILE *paperfile){
        //End for checking openfile
        if(paperfile)
            printf("The fileOpen is OK.\n");
        else
        {
            printf("Wrong\n");
        }//End for checking openfile
}
