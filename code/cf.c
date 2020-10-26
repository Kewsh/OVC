#include "main.h"

void cf(const char *path2, const char *path3){

  // c1 represents signs (+, -, .) and c2 represents other characters
  char c, c1, c2, c3, numstr[8];
  int i, k, num, loc1 = 0, temp = 0;
  FILE *fptr1, *fptr3, *fptr4;

  fptr1 = fopen("_CTRLDIR/temps/fullVrs.txt", "r");
  fptr3 = fopen(path2, "r");
  fptr4 = fopen(path3, "w");

  while(1){
    c1 = fgetc(fptr3);
    switch(c1){
      case '+':                                                                                              // insertions from file2
        k = 0;
        while ((c2 = fgetc(fptr3)) != ' ')
          numstr[k++] = c2;
        numstr[k] = '\0';
        num = atoi(numstr);
        for (i = 0; i < num; i++){
          c2 = fgetc(fptr3);
          fputc(c2, fptr4);
        }
        fgetc(fptr3);
        break;
      case '-':                                                                                              // deletions from file1
        k = 0;
        while ((c2 = fgetc(fptr3)) != '\n')
          numstr[k++] = c2;
        numstr[k] = '\0';
        num = atoi(numstr);
        for (i = 0; i < num; i++){
          fgetc(fptr1);
          loc1++;
        }
        loc1 += num;
        break;
      case '.':                                                                                              // forwards from LCS
        k = 0;
        while ((c2 = fgetc(fptr3)) != '\n')
          numstr[k++] = c2;
        numstr[k] = '\0';
        num = atoi(numstr);
        if (num == 256)
          num--;
        for (i = 0; i < num+1; i++){
          c2 = fgetc(fptr1);
          fputc(c2, fptr4);
          loc1++;
        }
        break;
      default:                                                                                               // EOF
        temp = 1;
    }
    if (temp == 1)
      break;
  }

  fclose(fptr1);
  fclose(fptr3);
  fclose(fptr4);
}