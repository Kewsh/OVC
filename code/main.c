#include "main.h"

/*
 * __validateEntry__ validates the input command string by breaking it into several tokens
 * and then calling the necessary functions to act based on the command
 * returns 1 if command is __init__, 0 upon success of any other command, -1 if input is invalid
 */
int validateEntry(char *userCommand, int isInitialized){
  char delimiter[] = " ", commitDesc[MAX_COMMITDESC_SIZE] = "";
  char c, *token, prompt;
  int k, resetID, stashID, stashFlag = 0, temp = 0;
  FILE *fptr;
  token = strtok(userCommand, delimiter);                                                                    // tokenizing the command string by single spaces
  if (token == NULL)                                                                                         // empty string
    return -1;
  if (strcmp(token, "select") == 0 || strcmp(token, "unselect") == 0 || strcmp(token, "stash") == 0 ||
      strcmp(token, "reset") == 0 || strcmp(token, "log") == 0 || strcmp(token, "status") == 0 ||
      strcmp(token, "commit") == 0)
    temp = 1;
  if (isInitialized == 1){
    fptr = fopen("_CTRLDIR/flags/stashFlag.txt", "r");
    fscanf(fptr, "%d", &stashFlag);
    fclose(fptr);
  }
  if (strcmp(token, "select") == 0 && isInitialized == 1 && stashFlag == 0){
    token = strtok(NULL, delimiter);
    if (token == NULL){                                                                                      // invalid use
      SetConsoleTextAttribute(console, RED);
      printf("ERR. invalid use\n");
      SetConsoleTextAttribute(console, DEFAULT);
      printf("__select__ takes at least one argument; try:\n");
      printf("  ~select -all\n  ~select -list\n  ~select [file name]\n");
      return -1;
    }
    else if (strcmp(token, "-all") == 0){                                                                    // "select -all"
      selectAll();
      return 0;
    }
    else if (strcmp(token, "-list") == 0){                                                                   // "select -list"
      selectList();
      return 0;
    }
    else{                                                                                                    // "select [file name]"
      selectFile(token);
      return 0;
    }
  }
  else if (strcmp(token, "unselect") == 0 && isInitialized == 1){
    token = strtok(NULL, delimiter);
    if (token == NULL){                                                                                      // invalid use
      SetConsoleTextAttribute(console, RED);
      printf("ERR. invalid use\n");
      SetConsoleTextAttribute(console, DEFAULT);
      printf("__unselect__ takes at least one argument; try:\n");
      printf("  ~unselect -all\n  ~unselect [file name]\n");
      return -1;
    }
    if (strcmp(token, "-all") == 0){                                                                         // "unselect -all"
      unselectAll();
      return 0;
    }
    else{                                                                                                    // "unselect [file name]"
      unselectFile(token);
      return 0;
    }
  }
  else if (strcmp(token, "stash") == 0 && isInitialized == 1){
    token = strtok(NULL, delimiter);
    if (token == NULL){                                                                                      // "stash"
      fptr = fopen("_CTRLDIR/commit/commitID.txt", "r");
      fscanf(fptr, "%d", &commitID);
      fclose(fptr);
      stash(commitID);
      return 0;
    }
    if (strcmp(token, "pop") == 0){                                                                          // "stash pop"
      stashPop();
      return 0;
    }
    else{                                                                                                    // "stash [commit ID]"
      stashID = atoi(token);
      stash(stashID);
      return 0;
    }
  }
  else if (strcmp(token, "reset") == 0 && isInitialized == 1 && stashFlag == 0){
    token = strtok(NULL, delimiter);
    if (token != NULL){                                                                                      // "reset [commit ID]"
      resetID = atoi(token);
      printf("please confirm that you want to reset to the target commit\n[y/n]: ");
      while(1){
        prompt = getchar();
        getchar();
        if (prompt == 'y')
          break;
        else if (prompt == 'n')
          return -1;
        SetConsoleTextAttribute(console, RED);
        printf("ERR. invalid input\n");
        SetConsoleTextAttribute(console, DEFAULT);
        printf("[y/n]: ");
        fflush(stdin);
      }
      reset(resetID);
      return 0;
    }
    else{                                                                                                    // invalid use
      SetConsoleTextAttribute(console, RED);
      printf("ERR. no specific commit ID was mentioned\n");
      SetConsoleTextAttribute(console, DEFAULT);
      printf("  ~reset [commit ID]\n");
      return -1;
    }
  }
  else if (strcmp(token, "log") == 0 && isInitialized == 1){                                                 // "log"
    printLog();
    return 0;
  }
  else if (strcmp(token, "status") == 0 && isInitialized == 1){
    status();
    return 0;
  }
  else if (strcmp(token, "init") == 0){                                                                      // "init"
    init();
    return 1;
  }
  else if (strcmp(token, "commit") == 0 && isInitialized == 1 && stashFlag == 0){
    token = strtok(NULL, delimiter);
    if (token == NULL){                                                                                      // commit
      printf("commit message: ");
      k = 0;
      while((c = getchar()) != '\n')
        commitDesc[k++] = c;
      commitDesc[k] = '\0';
      commit(commitDesc);
      return 0;
    }
    else if (strcmp(token, "-m") == 0){                                                                      // "commit -m [message]"
      token = strtok(NULL, delimiter);
      if (token == NULL){                                                                                    // invalid use
        SetConsoleTextAttribute(console, RED);
        printf("ERR. no commit message was given\n");
        SetConsoleTextAttribute(console, DEFAULT);
        printf("  ~commit -m [message]\n  ~commit\n");
        return -1;
      }
      printf("please confirm that you want to commit\n[y/n]: ");
      while(1){
        prompt = getchar();
        getchar();
        if (prompt == 'y')
          break;
        else if (prompt == 'n')
          return -1;
        SetConsoleTextAttribute(console, RED);
        printf("ERR. invalid input\n");
        SetConsoleTextAttribute(console, DEFAULT);
        printf("[y/n]: ");
        fflush(stdin);
      }
      do{
        strcat(commitDesc, token);
        strcat(commitDesc, " ");
      }while((token = strtok(NULL, delimiter)) != NULL);
      commitDesc[strlen(commitDesc)-1] = '\0';
      commit(commitDesc);
      return 0;
    }
    else{                                                                                                    // "commit"
      printf("commit message: ");
      k = 0;
      while((c = getchar()) != '\n')
        commitDesc[k++] = c;
      commitDesc[k] = '\0';
      commit(commitDesc);
      return 0;
    }
  }
  else if (strcmp(token, "help") == 0){
    token = strtok(NULL, delimiter);
    if (token == NULL){                                                                                      // "help"
      help("");
    }
    else{                                                                                                    // "help [command]"
      help(token);
    }
    return 0;
  }
  else if (strcmp(token, "exit") == 0){                                                                      // "exit"
    exit(0);
  }
  else if (temp == 1 && stashFlag == 0){
    SetConsoleTextAttribute(console, RED);
    printf("ERR. repository hasn't been initialized.\n");
    SetConsoleTextAttribute(console, DEFAULT);
    printf("  ~init\n");
    return -1;
  }
  else if (temp == 0){                                                                                       // invalid command
    SetConsoleTextAttribute(console, RED);
    printf("ERR. invalid command\n");
    SetConsoleTextAttribute(console, DEFAULT);
    printf("list of commands:\n");
    printf("  ~select\n  ~unselect\n  ~stash\n  ~reset\n  ~log\n  ~status\n  ~init\n  ~commit\n  ~help\n  ~exit\n");
    return -1;
  }
  else{
    SetConsoleTextAttribute(console, RED);
    printf("ERR. this command cannot be executed until stash has been popped\n");
    SetConsoleTextAttribute(console, DEFAULT);
    printf("  ~stash pop\n");
    return -1;
  }
}

/*
 * __main__
 */
int main(){
  console  = GetStdHandle(STD_OUTPUT_HANDLE);
  int isInitialized = 0, result, k;
  char c, userCommand[MAX_CMDSTR_SIZE];
  DIR *mainDir;
  struct dirent *entry;
  SetConsoleTextAttribute(console, PURPLE);
  printf("\nOmelette v1.0");
  SetConsoleTextAttribute(console, DEFAULT);
  do{
    printf("\n\nproject folder path: ");
    k = 0;
    while((c = getchar()) != '\n')
      prjPath[k++] = c;
    prjPath[k] = '\0';                                                                                       // getting the main project directory from the user
  }while((mainDir = opendir(prjPath)) == NULL);                                                              // invalid path
  printf("\nroot is %s\n\n", prjPath);
  closedir(mainDir);
  chdir(prjPath);
  if (checkInitilization() == 0)
    isInitialized = 1;
  while(1){                                                                                                  // the main loop of the program
    SetConsoleTextAttribute(console, PURPLE);
    printf("->$:");
    SetConsoleTextAttribute(console, DEFAULT);
    k = 0;
    while((c = getchar()) != '\n')
      userCommand[k++] = c;
    userCommand[k] = '\0';
    result = validateEntry(userCommand, isInitialized);
    if (result == 1)                                                                                         // init has been called
      isInitialized = 1;
    continue;
  }
  return 0;
}