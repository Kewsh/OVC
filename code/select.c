#include "main.h"

/*
 * __strtoLower__ takes a string as input and sets all its characters to lowercase
 * returns a pointer pointing to the first character of the temp string
 */
char *strtoLower(const char *string){
  char c, *temp = malloc((strlen(string)+1)*sizeof(char));
  int i, j = 0;
  for (i = 0; i < strlen(string); i++){
    c = string[i];
    if (c >= 65 && c <= 90)
      c += 32;
    temp[j++] = c;
  }
  temp[j] = '\0';
  return temp;
}

/*
 * __selectFile__ selects files to be commited
 * "select -all" to select all files
 * "select -list" to view a list of the selected files
 * returns 0 upon success, -1 if faced errors
 */
int selectFile(const char *fullFilePath){
  FILE *fptr1 = fopen(fullFilePath, "r"), *fptr2, *fptr3;
  int flag, result;
  char *temp = strtoLower(fullFilePath);
  if (fptr1 == NULL){
    SetConsoleTextAttribute(console, RED);
    printf("ERR. Failed to locate file/Access denied\n");
    SetConsoleTextAttribute(console, DEFAULT);
    return -1;
  }
  else if (strstr(temp, "_ctrldir") != NULL || strstr(temp, "../") != NULL
        || strstr(temp, "_originfiles") != NULL || strstr(temp, ":") != NULL){
    SetConsoleTextAttribute(console, RED);
    printf("ERR. Invalid file path/name\n");
    SetConsoleTextAttribute(console, DEFAULT);
    printf("note: root is %s\n", prjPath);
    return -1;
  }
  else if ((result = checkFile(fullFilePath)) == -1){
    SetConsoleTextAttribute(console, RED);
    printf("ERR. duplicate file names not allowed\n");
    SetConsoleTextAttribute(console, DEFAULT);
    fclose(fptr1);
    return -1;
  }
  else{
    fptr2 = fopen("_CTRLDIR/flags/selectFlag.txt", "r+");
    fscanf(fptr2, "%d", &flag);
    if (flag == 0){                                                                                          // turning the select flag ON
      fseek(fptr2, ftell(fptr2)-1, SEEK_SET);
      fputc('1', fptr2);
      fptr3 = fopen("_CTRLDIR/temps/selectedFiles.txt", "w");
      fprintf(fptr3, "%s\n%d %c\n", fullFilePath, result, 's');                                              // first selected file, no need to check anything, so we add it
      fclose(fptr3);
      fclose(fptr2);
      fclose(fptr1);
      return 0;
    }
    fclose(fptr2);

    char state, c, tempPathName[MAX_FULLPATH_SIZE];
    int k;
    fptr2 = fopen("_CTRLDIR/temps/selectedFiles.txt", "r+");                                                 // checking whether the file has already been mentioned (s/u)
    while(1){
      k = 0;
      while((c = fgetc(fptr2)) != '\n' && c != EOF)
        tempPathName[k++] = c;
      if (c == EOF)
        break;
      tempPathName[k] = '\0';
      if (strcmp(tempPathName, fullFilePath) == 0){
        fgetc(fptr2), fgetc(fptr2);                                                                          // getting rid of the mode digit and the single space character
        state = fgetc(fptr2);                                                                                // current selection state ('s' or 'u')
        if (state == 's'){                                                                                   // already selected
          printf("this file has already been selected\n");
          fclose(fptr2);
          fclose(fptr1);
          return -1;
        }
        else{                                                                                                // previously unselected, now selected
          fseek(fptr2, ftell(fptr2)-1, SEEK_SET);
          fputc('s', fptr2);
          fclose(fptr2);
          fclose(fptr1);
          return 0;
        }
        fgetc(fptr2);                                                                                        // getting rid of the newline character
      }
      else
        fgetc(fptr2), fgetc(fptr2), fgetc(fptr2), fgetc(fptr2);                                              // getting rid of the current index data
    }
    fprintf(fptr2, "%s\n%d %c\n", fullFilePath, result, 's');                                                // file hasnt been mentioned, so we add it
    fclose(fptr2);
  }
  fclose(fptr1);
  return 0;
}

/*
 * __selectAllRecursive__ recursively opens directories and adds their file names
 * full paths to selectedFiles.txt
 */
void selectAllRecursive(DIR *directory, FILE *filePointer, const char *backwardPathName, int *cp){
  struct dirent *entry;
  DIR *subDirectory;
  int result, templen;
  char *temp, fullFilePath[MAX_WIDE_FULLPATH_SIZE];
  while((entry = readdir(directory)) != NULL){
    if (strcmp(entry->d_name, "_CTRLDIR") == 0)
      continue;
    if (strcmp(entry->d_name, ".") == 0)
      continue;
    if (strcmp(entry->d_name, "..") == 0)
      continue;
    sprintf(fullFilePath, "%s%s", backwardPathName, entry->d_name);
    if ((subDirectory = opendir(fullFilePath)) != NULL){                                                     // it's a directory
      templen = strlen(fullFilePath);
      fullFilePath[templen++] = '/';
      fullFilePath[templen] = '\0';
      selectAllRecursive(subDirectory, filePointer, fullFilePath, cp);                                       // work our way up recursively
    }
    else{                                                                                                    // it's a file
      temp = strtoLower(fullFilePath);
      if ((result = checkFile(fullFilePath)) == -1){
        SetConsoleTextAttribute(console, RED);
        printf("ERR. duplicate file found\n");
        SetConsoleTextAttribute(console, DEFAULT);
        printf("  skipped (%s)\n", fullFilePath);
      }
      else if (strstr(temp, "_ctrldir") != NULL || strstr(temp, "_originfiles") != NULL){
        SetConsoleTextAttribute(console, RED);
        printf("ERR. Invalid file path/name\n");
        SetConsoleTextAttribute(console, DEFAULT);
        printf("  skipped (%s)\n", fullFilePath);
        printf("note: root is %s\n", prjPath);
      }
      else{
        (*cp)++;
        fprintf(filePointer, "%s\n%d %c\n", fullFilePath, result, 's');
      }
    }
  }
  closedir(directory);
}

/*
 * __selectAll__ calls the recursive function selectAllRecursive to do the job
 */
void selectAll(void){
  DIR *mainDir;
  FILE *fptr1, *fptr2;
  int flag, count = 0;
  int *cp = &count;
  fptr2 = fopen("_CTRLDIR/flags/selectFlag.txt", "r+");
  fscanf(fptr2, "%d", &flag);
  if (flag == 0){                                                                                            // turning the select flag ON
	  fseek(fptr2, ftell(fptr2)-1, SEEK_SET);
    fputc('1', fptr2);
  }
  else
    remove("_CTRLDIR/temps/selectedFiles.txt");
  fclose(fptr2);
  fptr1 = fopen("_CTRLDIR/temps/selectedFiles.txt", "w");
  mainDir = opendir(".");
  selectAllRecursive(mainDir, fptr1, "", cp);
  fclose(fptr1);
  if (count == 0){
    remove("_CTRLDIR/temps/selectedFiles.txt");
    fptr2 = fopen("_CTRLDIR/flags/selectFlag.txt", "w");
    fputc('0', fptr2);
    fclose(fptr2);
    printf("No files were selected\n");
    return;
  }
  SetConsoleTextAttribute(console, GREEN);
  printf("All files have been selected\n");
  SetConsoleTextAttribute(console, DEFAULT);
}

/*
 * __selectList__ lists the files selected by the user
 */
void selectList(void){
  FILE *fptr;
  char c, tempPathName[MAX_FULLPATH_SIZE];
  int k;
  char state;
  fptr = fopen("_CTRLDIR/temps/selectedFiles.txt", "r");
  if (fptr == NULL){
    printf("No files have been selected yet! Nothing to display\n");
    return;
  }
  while(1){
    k = 0;
    while ((c = fgetc(fptr)) != '\n' && c != EOF)
      tempPathName[k++] = c;
    if (c == EOF)
      break;
    tempPathName[k] = '\0';
    fgetc(fptr), fgetc(fptr);                                                                                // useless data
    state = fgetc(fptr);
    if (state == 's')
      printf("  ~%s\n", tempPathName);
    fgetc(fptr);                                                                                             // newline character
  }
  fclose(fptr);
}