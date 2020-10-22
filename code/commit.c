#include "main.h"

/*
 * __Originate__ places an initial version of the file in _CTRLDIR\origin
 * also keeps a record of all origin files in _originFiles.txt
 */
void originate(const char *fullFilePath){
  FILE *fptr1, *fptr2;
  char c, temp[MAX_WIDE_FULLPATH_SIZE], tempPathName[MAX_FULLPATH_SIZE], backwardPathName[MAX_FULLPATH_SIZE];
  int i, j = 0, templen;
  strcpy(backwardPathName, "_CTRLDIR/origin/");
  for (i = 0; i < strlen(fullFilePath); i++){
    if (fullFilePath[i] == '/'){
      tempPathName[j] = '\0';
      sprintf(temp, "%s%s", backwardPathName, tempPathName);
      _mkdir(temp);
      strcat(backwardPathName, tempPathName);
      templen = strlen(backwardPathName);
      backwardPathName[templen++] = '/';
      backwardPathName[templen] = '\0';
      j = 0;
    }
    else
      tempPathName[j++] = fullFilePath[i];
  }
  sprintf(temp, "_CTRLDIR/origin/%s", fullFilePath);
  fptr1 = fopen(temp, "w");
  fptr2 = fopen(fullFilePath, "r");
  while((c = fgetc(fptr2)) != EOF){
    fputc(c, fptr1);
  }
  fclose(fptr1);
  fclose(fptr2);
  fptr1 = fopen("_CTRLDIR/origin/_originFiles.txt", "a");
  fprintf(fptr1, "%s\n%d\n", fullFilePath, commitID);                                                        // adding the originated file's name and origin ID to an archive
  fclose(fptr1);
}

/*
 * __launchDiff__ puts the full path of the file and commitID in _CTRLDIR\temps\diffPipe.txt
 * for diff.c to read and locate the file, it also compiles and runs diff.c; then it launches
 * cf.c the same way to create the full version of the file as well and then after its work
 * is done, __launchDiff__ deletes diffPipe.txt
 */
void launchDiff(const char *fullFilePath){
  char c, path[MAX_EXTRAWIDE_FULLPATH_SIZE];
  FILE *fptr, *fptr2 = fopen("_CTRLDIR/temps/diffPipe.txt", "w");
  sprintf(path, "_CTRLDIR/commit/commit%d/%s/%s", commitID-1, fullFilePath, buildFileName(fullFilePath, 'f'));
  fptr = fopen(path, "r");
  if (fptr == NULL){                                                                                         // if we dont find the -f- version, it means that previous commit was the origin of this file
    sprintf(path, "_CTRLDIR/origin/%s", fullFilePath);
    fprintf(fptr2, "%s\n%s\n%d", path, fullFilePath, commitID);                                              // getting the file from origin
  }
  else
    fprintf(fptr2, "_CTRLDIR/commit/commit%d/%s/%s\n%s\n%d", commitID-1, fullFilePath, buildFileName(fullFilePath, 'f'), fullFilePath, commitID);
  fclose(fptr);
  fclose(fptr2);

  chdir(sourcePath);
  fptr = fopen("path.txt", "w");                                                                             // diff.c reads the project path from this file
  fprintf(fptr, "%s", prjPath);
  fclose(fptr);
  system("gcc -o diff diff.c");
  system("diff");
  remove("path.txt");
  chdir(prjPath);

  sprintf(path, "_CTRLDIR/commit/commit%d/%s/%s", commitID-1, fullFilePath, buildFileName(fullFilePath, 'f'));
  fptr = fopen(path, "r");
  if (fptr == NULL){
    sprintf(path, "_CTRLDIR/origin/%s", fullFilePath);
    fptr = fopen(path, "r");
  }
  fptr2 = fopen("_CTRLDIR/temps/fullVrs.txt", "w");
  while((c = fgetc(fptr)) != EOF)
    fputc(c, fptr2);
  fclose(fptr);
  fclose(fptr2);
  fptr = fopen("_CTRLDIR/temps/diffPipe.txt", "w");
  fprintf(fptr, "%s\n%d\n%d", fullFilePath, commitID, 2);
  fclose(fptr);

  chdir(sourcePath);
  fptr = fopen("path.txt", "w");                                                                             // cf.c reads the project path from this file
  fprintf(fptr, "%s", prjPath);
  fclose(fptr);
  system("gcc -o cf cf.c");
  system("cf");
  remove("path.txt");
  chdir(prjPath);

  remove("_CTRLDIR/temps/fullVrs.txt");
  remove("_CTRLDIR/temps/diffPipe.txt");
}

/*
 * __addFile__ inserts the snapshot of the file into the commit#ID directory
 * after creating all the required folders to place the file in its correct place.
 * also adds the full pathname of the commited files to commitedFiles.txt
 * and originates it if need be
 */
void addFile(const char *fullFilePath, int mode){
  char temp[MAX_WIDE_FULLPATH_SIZE], tempPathName[MAX_WIDE_FULLPATH_SIZE], backwardPath[MAX_FULLPATH_SIZE];
  int i, j = 0;
  FILE *fptr;
  sprintf(backwardPath, "_CTRLDIR/commit/commit%d/", commitID);
  for (i = 0; i < strlen(fullFilePath); i++){
    if (fullFilePath[i] == '/'){
      temp[j] = '\0';
      sprintf(tempPathName, "%s%s", backwardPath, temp);
      _mkdir(tempPathName);
      sprintf(backwardPath, "%s/", tempPathName);
      j = 0;
    }
    else
      temp[j++] = fullFilePath[i];
  }
  sprintf(temp, "_CTRLDIR/commit/commit%d/%s", commitID, fullFilePath);
  _mkdir(temp);
  if (mode == 1)                                                                                             // new file
    originate(fullFilePath);
  else                                                                                                       // modified file
    launchDiff(fullFilePath);
  sprintf(temp, "_CTRLDIR/commit/commit%d/commitedFiles.txt", commitID);                                     // saving the full pathnames of commited files
  fptr = fopen(temp, "a");
  fprintf(fptr, "%s\n", fullFilePath);
  fclose(fptr);
}

/*
 * __checkFile__ validates the possibility of commiting the mentioned fileName
 * if fileName exists in the previous commit, it will be saved as a modified file
 * if fileName exists neither in previous commit nor in origin, it will be saved as a new file
 * if fileName does not exist in previous commit but does exist in origin, it's a duplicate and it is not permitted for commit
 * if commitID is equal to 1, then it's the first ever commit, and the file will be saved as a new file
 * returns positive integer upon success(1: new file, 2: modified file), and -1 if faced errors
 */
int checkFile(const char *fileName){
  char c, path[MAX_FULLPATH_SIZE], temp[MAX_FULLPATH_SIZE];
  FILE *fptr;
  int isPresent = 0, childID, i;
  fptr = fopen("_CTRLDIR//commit//commitID.txt", "r");
  fscanf(fptr, "%d", &commitID);
  fclose(fptr);
  if(commitID >= 1){
    childID = getChildID(commitID);                                                                          // getting the child ID
    if (childID < 0){                                                                                        // file not found: childID is a reset commit
      childID = -getChildID(childID);                                                                        // the commit id that the reset was based on
      sprintf(path, "_CTRLDIR/commit/commit%d/commitedFiles.txt", childID);                                  // path for child commit commitFiles.txt
    }
    else
      sprintf(path, "_CTRLDIR/commit/commit%d/commitedFiles.txt", commitID);
    fptr = fopen(path, "r");
    while(1){
      i = 0;
      while((c = fgetc(fptr)) != EOF && c != '\n')
        temp[i++] = c;
      if (c == EOF)
        break;
      temp[i] = '\0';
      if (strcmp(temp, fileName) == 0){
        isPresent = 1;
        break;
      }
    }
    fclose(fptr);
    if (isPresent == 1){                                                                                     // modified file
      return 2;
    }
    else{
      fptr = fopen("_CTRLDIR/origin/_originFiles.txt", "r");
      while(1){
        i = 0;
        while((c = fgetc(fptr)) != EOF && c != '\n')
          temp[i++] = c;
        if (c == EOF)
          break;
        temp[i] = '\0';
        if (strcmp(temp, fileName) == 0){
          isPresent = 1;
          break;
        }
        while((c = fgetc(fptr)) != '\n');
      }
      fclose(fptr);
      if (isPresent == 0){                                                                                   // new file
        return 1;
      }
      else{                                                                                                  // duplicate names (error)
        return -1;
      }
    }
  }
  else                                                                                                       // first ever commit - new file
    return 1;
}

/*
 * __stageFiles__ stages the selected files for commit
 * returns the number of commited files
 */
int stageFiles(void){
  int mode, k, count = 0;
  char state, tempchar, c, tempPathName[MAX_FULLPATH_SIZE];
  FILE *fptr;
  fptr = fopen("_CTRLDIR/temps/selectedFiles.txt", "r");
  while(1){
    k = 0;
    while ((c = fgetc(fptr)) != '\n' && c != EOF)
      tempPathName[k++] = c;
    if (c == EOF)
      break;
    tempPathName[k] = '\0';
    tempchar = fgetc(fptr);
    mode = tempchar-48;                                                                                      // converting the character digit to a number
    fgetc(fptr);
    state = fgetc(fptr);
    fgetc(fptr);
    if (state == 's'){
      count++;
      addFile(tempPathName, mode);
    }
  }
  fclose(fptr);
  return count;
}

/*
 * __commit__ takes a snapshot of the repo at the time it is called
 * "commit -m [message]" to implicitly give the commit message to the program
 * returns 0 upon success, -1 if faced errors
 */
int commit(const char *commitDesc){
  FILE *fptr;
  int id, flag, count;
  char dirName[MAX_FILENAME_SIZE], tempPathName[MAX_FULLPATH_SIZE];
  fptr = fopen("_CTRLDIR/flags/selectFlag.txt", "r");
  fscanf(fptr, "%d", &flag);
  if (flag == 0){                                                                                            // making sure the user has selected at least 1 file for commit
    SetConsoleTextAttribute(console, RED);
    printf("ERR. no files have been selected for commit\n");
    SetConsoleTextAttribute(console, DEFAULT);
    fclose(fptr);
    return -1;
  }
  fclose(fptr);
  printf("\n>>> processing...\n\n");
  fptr = fopen("_CTRLDIR/commit/commitID.txt", "r");
  fscanf(fptr, "%d", &id);                                                                                   // getting current commit ID
  fclose(fptr);
  fptr = fopen("_CTRLDIR/commit/commitID.txt", "w");
  commitID = id+1;                                                                                           // updating commit ID
  fprintf(fptr, "%d", commitID);
  fclose(fptr);

  sprintf(dirName, "_CTRLDIR/commit/commit%d", commitID);                                                    // setting up the commit#ID directory
  _mkdir(dirName);
  sprintf(tempPathName, "%s/family.txt", dirName);
  fptr = fopen(tempPathName, "w");
  fprintf(fptr, "%d", commitID-1);                                                                           // setting up child ID
  fclose(fptr);

  count = stageFiles();                                                                                      // staging the selected files for commit
  remove("_CTRLDIR/temps/selectedFiles.txt");
  fptr = fopen("_CTRLDIR/flags/selectFlag.txt", "w");
  fputc('0', fptr);
  fclose(fptr);

  addToLog(commitDesc);                                                                                      // adding commit info to log.txt
  deleteFullVersions(commitID-2);                                                                            // no need for commitID-2's full versions anymore
  SetConsoleTextAttribute(console, GREEN);
  printf("successfully commited %d file(s)\n", count);
  SetConsoleTextAttribute(console, DEFAULT);
  return 0;
}