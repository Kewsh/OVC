#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_FULLPATH_SIZE 128

/*
 * builds a temp file name that contains the file mode (f/d) and not the path
 * returns a char pointer pointing to the first character of the string
 */
char *buildFileName(const char *fullFilePath, char mode){
  char *fpPTR, *temp = (char *)(malloc(MAX_FULLPATH_SIZE*sizeof(char)));
  fpPTR = (char *)(malloc(MAX_FULLPATH_SIZE*sizeof(char)));
  int i, j = 0, length = strlen(fullFilePath);
  for (i = length-1; i >= 0; i--){
    if (fullFilePath[i] == '/')
      break;
    if (fullFilePath[i] == '.'){
      temp[j++] = '.';
      temp[j++] = '-';
      temp[j++] = mode;
      temp[j++] = '-';
    }
    else
      temp[j++] = fullFilePath[i];
  }
  temp[j] = '\0';
  j = 0;
  for (i = strlen(temp)-1; i >= 0; i--)
	  fpPTR[j++] = temp[i];
  fpPTR[j] = '\0';
  free(temp);
  return fpPTR;
}

int main(){

  // c1 represents signs (+, -, .) and c2 represents other characters
  char c, c1, c2, c3, numstr[8], tempDir1[MAX_FULLPATH_SIZE], path[MAX_FULLPATH_SIZE];
  int i, k, num, loc1 = 0, temp = 0, targetID, mode;
  FILE *fptr1, *fptr3, *fptr4;
  FILE *tempPathPTR;
  char prjPath[MAX_FULLPATH_SIZE], path2[MAX_FULLPATH_SIZE];
	tempPathPTR = fopen("path.txt", "r");
  k = 0;
  while((c = fgetc(tempPathPTR)) != EOF)
    prjPath[k++] = c;
  prjPath[k] = '\0';
	fclose(tempPathPTR);
	chdir(prjPath);

  FILE *diffPipePTR = fopen("_CTRLDIR/temps/diffPipe.txt", "r");
  k = 0;
  while((c = fgetc(diffPipePTR)) != '\n')
    tempDir1[k++] = c;
  tempDir1[k] = '\0';
  fscanf(diffPipePTR, "%d\n%d", &targetID, &mode);
  sprintf(path, "_CTRLDIR/commit/commit%d/%s/%s", targetID, tempDir1, buildFileName(tempDir1, 'd'));
  if (mode == 1)
    sprintf(path2, "%s", tempDir1);
  else if (mode == 2)
    sprintf(path2, "_CTRLDIR/commit/commit%d/%s/%s", targetID, tempDir1, buildFileName(tempDir1, 'f'));
  else
    sprintf(path2, "_CTRLDIR/temps/fullVrs2.txt");
  fclose(diffPipePTR);

  fptr1 = fopen("_CTRLDIR/temps/fullVrs.txt", "r");
  fptr3 = fopen(path, "r");
  fptr4 = fopen(path2, "w");

  while(1){
    c1 = fgetc(fptr3);
    switch(c1){
      case '+':                           // insertions from file2
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
      case '-':                           // deletions from file1
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
      case '.':                           // forwards from LCS
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
      default:                            // EOF
        temp = 1;
    }
    if (temp == 1)
      break;
  }

  fclose(fptr1);
  fclose(fptr3);
  fclose(fptr4);

  return 0;
}
