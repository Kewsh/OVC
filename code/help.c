#include "main.h"

/*
 * __help__
 */
void help(const char *command){
  if (strlen(command) == 0){
    printf("Omelette is a CMD-based version control system\nuse Omelette to keep track of your files and recover them if necessary\n");
    printf("list of available commands:\n");
    printf("  ~select\n  ~unselect\n  ~stash\n  ~reset\n  ~log\n  ~status\n  ~init\n  ~commit\n  ~help\n  ~exit\n");
    printf("use help [command] for more info about a specific command\n");
  }
  else if (strcmp(command, "select") == 0){
    printf("use this command to select files to commit:\n  ~select -all\n  ~select -list\n  ~select [file name]\n");
    printf("note: this command doesnt take directory paths as arguments, only file paths are allowed\n");
  }
  else if (strcmp(command, "unselect") == 0){
    printf("use this command to unselect files:\n  ~unselect -all\n  ~unselect [filename]\n");
  }
  else if (strcmp(command, "stash") == 0){
    printf("by using this command, you can temporarily go back to a specific commit ID:\n  ~stash\n  ~stash [commit ID]\n  ~stash pop\n");
    printf("note: you WILL NOT lose any data when you use this command\n");
    printf("note: when you use this command, you will not be able to use the following commands until you pop the stash\n");
    printf("  ~select\n  ~reset\n  ~commit\n  ~stash\n");
    printf("note: to pop the stash, use: ~stash pop\n");
  }
  else if (strcmp(command, "reset") == 0){
    printf("this command takes you back to a specific commit by recreating your project folder:\n  ~reset [commit ID]\n");
    printf("note: you WILL lose any uncommited data when you use this command\n");
  }
  else if (strcmp(command, "log") == 0){
    printf("this command prints the logs of your repository\n");
  }
  else if (strcmp(command, "status") == 0){
    printf("this command prints the current status of your files\n");
  }
  else if (strcmp(command, "init") == 0){
    printf("this command initialises the repository by creating a folder named <_CTRLDIR> in your project folder\n");
  }
  else if (strcmp(command, "commit") == 0){
    printf("use this command to save a snapshot of your selected files:\n  ~commit -m [message]\n  ~commit\n");
  }
  else if (strcmp(command, "help") == 0){
    printf("use this command for more detail about a given command:\n  ~help [command]\n");
  }
  else if (strcmp(command, "exit") == 0){
    printf("exit the program\n");
  }
  else{
    SetConsoleTextAttribute(console, RED);
    printf("ERR. invalid command\n");
    SetConsoleTextAttribute(console, DEFAULT);
  }
}