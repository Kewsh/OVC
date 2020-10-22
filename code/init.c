#include "main.h"

/*
 * __init__ creates the control panel of the repo
 */
void init(void){
  FILE *fptr;
  int res;
  res = _mkdir("_CTRLDIR");
  if (res == -1){
    printf("repository has already been initialized\n");
    return;
  }
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