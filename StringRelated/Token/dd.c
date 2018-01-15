
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main()
{
  char** a=NULL;
  a=(char**)malloc(sizeof(char*)*5);
  a[0]="23123";
  a[1]="sdasdasd";
  printf("%s,%s",a[0],a[1]);

  free(a);




  return 0;
}
