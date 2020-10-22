#include "main.h"

/*
 * __copyBtoM__ recursively creates all directories and copies all files from stashBackup
 * to the main project folder, at the same time, __CopyBtoM__ also empties stashBackup
 * by removing all the files and folders
 */
void copyBtoM(DIR *directory, const char *backwardPath){
  char c, path[MAX_FULLPATH_SIZE], backPath2[MAX_FULLPATH_SIZE], temp[MAX_FULLPATH_SIZE];
  DIR *subDirectory;
  FILE *fptr1, *fptr2;
  struct dirent *entry;
  while((entry = readdir(directory)) != NULL){
    if (strcmp(entry->d_name, ".") == 0)
      continue;
    if (strcmp(entry->d_name, "..") == 0)
      continue;
    sprintf(path, "_CTRLDIR/temps/stashBackup/%s%s", backwardPath, entry->d_name);
    if ((fptr1 = fopen(path, "r")) != NULL){
      sprintf(temp, "%s%s", backwardPath, entry->d_name);
      fptr2 = fopen(temp, "w");
      while((c = fgetc(fptr1)) != EOF)
        fputc(c, fptr2);
      fclose(fptr1);
      fclose(fptr2);
      remove(path);
    }
    else{
      sprintf(backPath2, "%s%s/", backwardPath, entry->d_name);
      mkdir(backPath2);
      subDirectory = opendir(path);
      copyBtoM(subDirectory, backPath2);
    }
  }
  sprintf(path, "_CTRLDIR/temps/stashBackup/%s", backwardPath);
  rmdir(path);
  closedir(directory);
}

/*
 * __stashPop__ pops the last stash if stashFlag is set to 1. then deletes all files
 * in main project folder and then recreates it exactly like it was before stash was called
 * which means that even the unselected and the uncommited files will be present and will
 * look the same as before stash was called
 * returns 0 upon success, -1 if faced errors
 */
int stashPop(void){
  int stashFlag;
  FILE *fptr;
  DIR *directory;
  fptr = fopen("_CTRLDIR/flags/stashFlag.txt", "r");
  fscanf(fptr, "%d", &stashFlag);
  fclose(fptr);
  if (stashFlag == 0){
    printf("pop failed, no recent stash to pop.\n");
    return -1;
  }
  fptr = fopen("_CTRLDIR/flags/stashFlag.txt", "w");
  fputc('0', fptr);                                                                                          // setting the stash flag back to 0
  fclose(fptr);
  fptr = fopen("_CTRLDIR/temps/popID.txt", "r");
  fscanf(fptr, "%d", &commitID);
  fclose(fptr);
  remove("_CTRLDIR/temps/popID.txt");                                                                        // deleting the temporarily file that contains the original commit ID
  fptr = fopen("_CTRLDIR/commit/commitID.txt", "w");
  fprintf(fptr, "%d", commitID);                                                                             // resetting the commit ID
  fclose(fptr);

  directory = opendir(".");
  deleteFilesRecursively(directory, "", 1);
  directory = opendir("_CTRLDIR/temps/stashBackup");
  copyBtoM(directory, "");
  rmdir("_CTRLDIR/temps/stashBackup");
  SetConsoleTextAttribute(console, GREEN);
  printf("successfully popped the stash\n");
  SetConsoleTextAttribute(console, DEFAULT);
  return 0;
}

/*
 * __backupFiles__ holds a copy of every file in the main project directory in the
 * stashBackup folder in _CTRLDIR/temps for the next stash pop to use
 */
void backupFiles(DIR *directory, const char *backwardPath){
  char c, path[MAX_FULLPATH_SIZE], path2[MAX_WIDE_FULLPATH_SIZE];
  char temp[MAX_FULLPATH_SIZE], backPath[MAX_FULLPATH_SIZE];
  int i, j, templen;
  DIR *subDirectory;
  struct dirent *entry;
  FILE *fptr1, *fptr2;
  while((entry = readdir(directory)) != NULL){
    if (strcmp(entry->d_name, ".") == 0)
      continue;
    if (strcmp(entry->d_name, "..") == 0)
      continue;
    if (strcmp(entry->d_name, "_CTRLDIR") == 0)
      continue;
    sprintf(path, "%s%s", backwardPath, entry->d_name);
    if ((fptr1 = fopen(path, "r")) != NULL){
      j = 0, templen = 0;
      sprintf(backPath, "_CTRLDIR/temps/stashBackup/");
      for (i = 0; i < strlen(path); i++){
        if (path[i] == '/'){
          temp[j] = '\0';
          strcat(backPath, temp);
          templen = strlen(backPath);
          backPath[templen] = '/';
          backPath[templen+1] = '\0';
          mkdir(backPath);
          j = 0;
        }
        else
          temp[j++] = path[i];
      }
      sprintf(path2, "_CTRLDIR/temps/stashBackup/%s", path);
      fptr2 = fopen(path2, "w");
      while((c = fgetc(fptr1)) != EOF)
        fputc(c, fptr2);
      fclose(fptr1);
      fclose(fptr2);
    }
    else{
      strcat(path, "/");
      subDirectory = opendir(path);
      backupFiles(subDirectory, path);
    }
  }
  closedir(directory);
}

/*
 * __stash__ takes the user back to the target commit directory temporarily
 * which means the project directory will look exactly as it was commited back then
 * and if no target commit ID is mentioned, target will be the last commit, by default.
 * when called, __stash__ sets the stashFlag to 1 which means that from now on, the
 * user will not be able to execute specific commands until they've popped the stash
 * returns 0 upon success, -1 if faced errors
 */
int stash(int targetCommitID){
  char path[MAX_FULLPATH_SIZE], backPath2[MAX_WIDE_FULLPATH_SIZE], **targetPaths;
  char pathsArchive[MAX_FILE_COUNT][MAX_FULLPATH_SIZE];
  int minID, pathLength, archiveLength = 0, *archLenPointer, childID;
  int pathArr[MAX_RESETPATH_SIZE], originsArchive[MAX_FILE_COUNT], cmtID, index, i, temp, read, result, stashFlag;
  DIR *mainDir, *dir2;
  FILE *fptr;
  struct dirent *entry;

  fptr = fopen("_CTRLDIR/commit/commitID.txt", "r");
  fscanf(fptr, "%d", &commitID);                                                                             // getting current commit ID
  fclose(fptr);
  if (targetCommitID > commitID || targetCommitID <= 0){                                                     // the given commit ID for stash can not be bigger than the current commit ID, or non-positive
    SetConsoleTextAttribute(console, RED);
    printf("ERR. invalid commit ID for stash\n");
    SetConsoleTextAttribute(console, DEFAULT);
    return -1;
  }
  result = getChildID(targetCommitID);
  if (result < 0){                                                                                           // target id is a reset commit, cannot stash to a reset commit
    printf("stash failed. target commit ID is a RESET commit\n-->Reset from: commit#%d\n", -result);
    return -1;
  }
  fptr = fopen("_CTRLDIR/flags/stashFlag.txt", "r+");
  fscanf(fptr, "%d", &stashFlag);
  if (stashFlag == 1){
    SetConsoleTextAttribute(console, RED);
    printf("ERR. you need to pop the stash before using this command\n");
    SetConsoleTextAttribute(console, DEFAULT);
    printf("  ~stash pop\n");
    return -1;
  }
  printf("\n>>> processing...\n\n");
  fseek(fptr, ftell(fptr)-1, SEEK_SET);
  fputc('1', fptr);                                                                                          // turning on the stash flag
  fclose(fptr);
  fptr = fopen("_CTRLDIR/commit/commitID.txt", "w");
  fprintf(fptr, "%d", targetCommitID);                                                                       // temporarily changing the commit ID
  fclose(fptr);
  fptr = fopen("_CTRLDIR/temps/popID.txt", "w");
  fprintf(fptr, "%d", commitID);                                                                             // saving the original commit ID in a temp file
  fclose(fptr);

  _mkdir("_CTRLDIR/temps/stashBackup");
  mainDir = opendir(".");
  backupFiles(mainDir, "");
  mainDir = opendir(".");
  deleteFilesRecursively(mainDir, "", 1);                                                                    // emptying the current project directory
  reCreateDirectory(targetCommitID);                                                                         // recreating the project directory
  childID = getChildID(commitID);

  if (targetCommitID == commitID-1 || targetCommitID == childID){                                           // if target is either the child commit or the current commit
    sprintf(path, "_CTRLDIR/commit/commit%d", targetCommitID);
    mainDir = opendir(path);
    while((entry = readdir(mainDir)) != NULL){
      if (strcmp(entry->d_name, ".") == 0)
        continue;
      if (strcmp(entry->d_name, "..") == 0)
        continue;
      if (strcmp(entry->d_name, "family.txt") == 0)
        continue;
      if (strcmp(entry->d_name, "commitedFiles.txt") == 0)
        continue;
      sprintf(backPath2, "%s/%s", path, entry->d_name);
      dir2 = opendir(backPath2);
      sprintf(backPath2, "%s", entry->d_name);
      copyCtoM(dir2, path, backPath2);                                                                       // copying the contents of -f- commited files to files in main project directory
    }
    closedir(mainDir);
  }
  else{
    sprintf(path, "_CTRLDIR/commit/commit%d/commitedFiles.txt", targetCommitID);
    fptr = fopen(path, "r");
    archLenPointer = &archiveLength;
    minID = findMinID(archLenPointer, pathsArchive, originsArchive, fptr);                                   // finding the minimum origin ID to create the path
    pathLength = createResetPath(pathArr, minID, targetCommitID);                                            // creating the reset path and file path arrays
    temp = 0, read = 1;
    while(1){                                                                                                // begin creating files from -d- versions
      if (read == pathLength+1)
        break;
      if (temp == 1){                                                                                        // current commit is a reset commit, skip
        read++;
        temp = 0;
        continue;
      }
      cmtID = pathArr[pathLength-read];
      if (cmtID < 0){
        cmtID = -cmtID;
        temp = 1;
      }
      targetPaths = findInArchive(cmtID, archiveLength, pathsArchive, originsArchive);                       // array of files to be copied from origin
      index = 0;
      while(1){
        if (strlen(targetPaths[index]) == 0)
          break;
        copyOtoM(targetPaths[index++]);                                                                      // copy the file from origin to main directory
      }
      for (i = minID; i < cmtID; i++){
        targetPaths = findInArchive(i, archiveLength, pathsArchive, originsArchive);                         // array of files to be created with diff
        index = 0;
        while(1){
          if (strlen(targetPaths[index]) == 0)
            break;
          createFile(targetPaths[index++], cmtID);                                                           // create the new file using the -d- version
        }
      }
      read++;
    }
  }
  SetConsoleTextAttribute(console, GREEN);
  printf("successfully stashed to commit %d\n", targetCommitID);
  SetConsoleTextAttribute(console, DEFAULT);
  return 0;
}