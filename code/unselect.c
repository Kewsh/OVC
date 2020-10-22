#include "main.h"

/*
 * __unselectFile__ unselects the file that has been previously selected for commit
 * "unselect -all" to unselect all selected files
 * returns 0 upon success, -1 if faced errors
 */
int unselectFile(const char *fullFilePath){
  int flag, k;
  FILE *fptr;
  char state, c, tempPathName[MAX_FULLPATH_SIZE];
  fptr = fopen("_CTRLDIR/flags/selectFlag.txt", "r");
  fscanf(fptr, "%d", &flag);
  fclose(fptr);
  if (flag == 0){
    SetConsoleTextAttribute(console, RED);
    printf("ERR. No files have been selected yet\n");
    SetConsoleTextAttribute(console, DEFAULT);
    return -1;
  }
  fptr = fopen("_CTRLDIR/temps/selectedFiles.txt", "r+");
  while(1){
    k = 0;
    while((c = fgetc(fptr)) != '\n' && c != EOF)
      tempPathName[k++] = c;
    if (c == EOF)
      break;
    tempPathName[k] = '\0';
    if (strcmp(tempPathName, fullFilePath) == 0){
      fgetc(fptr), fgetc(fptr);                                                                              // getting rid of the unneeded data
      state = fgetc(fptr);
      if (state == 'u'){
        SetConsoleTextAttribute(console, RED);
        printf("ERR. this file hasn't been selected\n");
        SetConsoleTextAttribute(console, DEFAULT);
        fclose(fptr);
        return -1;
      }
      else{
        fseek(fptr, ftell(fptr)-1, SEEK_SET);
        fputc('u', fptr);
        fclose(fptr);
        checkEmptiness();                                                                                    // check if all selected files have been unselected
        return 0;
      }
      fgetc(fptr);                                                                                           // getting rid of the newline character
    }
    else
      fgetc(fptr), fgetc(fptr), fgetc(fptr), fgetc(fptr);                                                    // getting the unneeded data out of the way
  }
  SetConsoleTextAttribute(console, RED);
  printf("ERR. file not found/not selected\n");
  SetConsoleTextAttribute(console, DEFAULT);
  fclose(fptr);
  return -1;
}

/*
 * __checkEmptiness__ checks whether the select list is empty, if it is,
 * checkEmptiness sets selectFlag to 0 and deletes selectedFiles.txt
 */
void checkEmptiness(void){
  FILE *fptr = fopen("_CTRLDIR/temps/selectedFiles.txt", "r");
  char state, c;
  while(1){
    while((c = fgetc(fptr)) != '\n' && c != EOF);                                                            // skip characters until we reach selection state character
    if (c == EOF)
      break;
    fgetc(fptr), fgetc(fptr);                                                                                // useless bytes
    state = fgetc(fptr);
    if (state == 's'){                                                                                       // select list is not empty
      fclose(fptr);
      return;
    }
    fgetc(fptr);                                                                                             // getting rid of the newline character
  }
  fclose(fptr);
  remove("_CTRLDIR/temps/selectedFiles.txt");                                                                // select list is empty, deleting the temp file
  fptr = fopen("_CTRLDIR/flags/selectFlag.txt", "w");                                                        // setting selectFlag back to 0
  fputc('0', fptr);
  fclose(fptr);
}

/*
 * __unselectAll__ unselects all of the files selected for commit
 */
void unselectAll(void){
  int flag;
  FILE *fptr = fopen("_CTRLDIR/flags/selectFlag.txt", "r+");
  fscanf(fptr, "%d", &flag);
  if (flag == 0){
    printf("No files have been selected, nothing to unselect\n");
    return;
  }
  else{
    fseek(fptr, ftell(fptr)-1, SEEK_SET);
    fputc('0', fptr);                                                                                        // setting selectFlag back to 0
  }
  fclose(fptr);
  remove("_CTRLDIR/temps/selectedFiles.txt");                                                                // deleting the temp file
  SetConsoleTextAttribute(console, GREEN);
  printf("All files have been unselected\n");
  SetConsoleTextAttribute(console, DEFAULT);
}