#include "main.h"

/*
 * __checkInitilization__ checks the given directory for _CTRLDIR and its subdirectories and files
 * if found, returns 0 and if not, returns -1
 */
int checkInitilization(void){
  DIR *directory;
  FILE *fptr;
  if ((directory = opendir("_CTRLDIR")) == NULL)
    return -1;
  closedir(directory);
  if ((directory = opendir("_CTRLDIR/origin")) == NULL)
    return -1;
  closedir(directory);
  if ((directory = opendir("_CTRLDIR/commit")) == NULL)
    return -1;
  closedir(directory);
  if ((directory = opendir("_CTRLDIR/temps")) == NULL)
    return -1;
  closedir(directory);
  if ((directory = opendir("_CTRLDIR/flags")) == NULL)
    return -1;
  closedir(directory);
  if ((fptr = fopen("_CTRLDIR/commit/commitID.txt", "r")) == NULL)
    return -1;
  fclose(fptr);
  if ((fptr = fopen("_CTRLDIR/flags/selectFlag.txt", "r")) == NULL)
    return -1;
  fclose(fptr);
  if ((fptr = fopen("_CTRLDIR/flags/stashFlag.txt", "r")) == NULL)
    return -1;
  fclose(fptr);
  if ((fptr = fopen("_CTRLDIR/origin/_originFiles.txt", "r")) == NULL)
    return -1;
  fclose(fptr);
  if ((fptr = fopen("_CTRLDIR/log.txt", "r")) == NULL)
    return -1;
  fclose(fptr);
  return 0;
}

/*
 * __init__ creates the control panel of the repo
 */
void init(void){
  FILE *fptr;
  if (checkInitilization() == 0){
    printf("repository has already been initialized\n");
    return;
  }
  _mkdir("_CTRLDIR");
  fptr = fopen("_CTRLDIR/log.txt", "w");
  fclose(fptr);
  _mkdir("_CTRLDIR/flags");
  fptr = fopen("_CTRLDIR/flags/selectFlag.txt", "w");
  fputc('0', fptr);
  fclose(fptr);
  fptr = fopen("_CTRLDIR/flags/stashFlag.txt", "w");
  fputc('0', fptr);
  fclose(fptr);
  _mkdir("_CTRLDIR/commit");
  fptr = fopen("_CTRLDIR/commit/commitID.txt", "w");
  fprintf(fptr, "0");
  fclose(fptr);
  _mkdir("_CTRLDIR/origin");
  fptr = fopen("_CTRLDIR/origin/_originFiles.txt", "w");
  fclose(fptr);
  _mkdir("_CTRLDIR/temps");
  SetConsoleTextAttribute(console, GREEN);
  printf("repository successfully initialized\n");
  SetConsoleTextAttribute(console, DEFAULT);
}