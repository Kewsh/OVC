#include "main.h"

/*
 * __printLog__ prints the logs for the commit history
 * log info contains id, message and date and time of a commit
 */
void printLog(void){
  FILE *fptr = fopen("_CTRLDIR/log.txt", "r");
  char c;
  int newLineCount = 0;
  while(1){
    while((c = fgetc(fptr)) != '\n' && c != EOF)
      putchar(c);
    if (c == EOF)
      break;
    putchar('\n');
    newLineCount++;
    if (newLineCount == 3){
      newLineCount = 0;
      printf("----------------------------------\n");                                                        // separating the commits
    }
  }
  fclose(fptr);
}

/*
 * __addToLog__ adds the commit info to log.txt
 */
void addToLog(const char *message){
  FILE *fptr;
  fptr = fopen("_CTRLDIR/log.txt", "a");
  fprintf(fptr, "-id: %d\n-m: %s\n-t: ", commitID, message);
  time_t t = time(NULL);                                                                                     // accessing the system's current data and time
  struct tm tm = *localtime(&t);
  fprintf(fptr, "%d-%02d-%02d %02d:%02d:%02d\n",                                                             // printf format for date and time
  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);                           // struct tm attributes to access date and time
  fclose(fptr);
}