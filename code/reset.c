#include "main.h"

/*
 * __placeFile__ recursively searches and creates given directories in the main project folder
 * and places a blank version of the files inside them
 */
void placeFile(DIR *directory, const char *backwardPath){
  char tempPath[MAX_EXTRAWIDE_FULLPATH_SIZE], temp[MAX_FULLPATH_SIZE], backPath3[MAX_FULLPATH_SIZE];
  char backPath2[MAX_FULLPATH_SIZE], temp2[MAX_FULLPATH_SIZE], bwpCOPY[MAX_FULLPATH_SIZE], *token;
  int i, j, count = 0, tmp;
  FILE *fptr;
  DIR *subDirectory;
  struct dirent *entry;
  while((entry = readdir(directory)) != NULL){
    if (strcmp(entry->d_name, ".") == 0){
      count++;
      continue;
    }
    if (strcmp(entry->d_name, "..") == 0){
      count++;
      continue;
    }
    count++;
    sprintf(tempPath, "%s/%s", backwardPath, entry->d_name);
    if ((fptr = fopen(tempPath, "r")) != NULL){                                                              // spotted a file
      fclose(fptr);
      backPath2[0] = '\0';
      strcpy(bwpCOPY, backwardPath);
      tmp = 1;
      token = strtok(bwpCOPY, "/");
      while((token = strtok(NULL, "/")) != NULL){
        if (tmp >= 3){
          strcat(backPath2, token);
          strcat(backPath2, "/");
        }
        tmp++;
      }
      backPath2[strlen(backPath2)-1] = '\0';
      j = 0;
      backPath3[0] = '\0';
      for (i = 0; i < strlen(backPath2); i++){
        if (backPath2[i] == '/'){
          temp[j] = '\0';
          strcat(backPath3, temp);
          strcat(backPath3, "/");
          _mkdir(backPath3);
          j = 0;
        }
        else
          temp[j++] = backPath2[i];
      }
      fptr = fopen(backPath2, "w");
      fclose(fptr);
    }
    else{
      subDirectory = opendir(tempPath);                                                                      // spotted a folder, continuing recursion
      placeFile(subDirectory, tempPath);
    }
  }
  if (count == 2){                                                                                           // target commit was the origin of this file
    backPath2[0] = '\0';
    strcpy(bwpCOPY, backwardPath);
    tmp = 1;
    token = strtok(bwpCOPY, "/");
    while((token = strtok(NULL, "/")) != NULL){
      if (tmp >= 3){
        strcat(backPath2, token);
        strcat(backPath2, "/");
      }
      tmp++;
    }
    backPath2[strlen(backPath2)-1] = '\0';
    j = 0;
    backPath3[0] = '\0';
    for (i = 0; i < strlen(backPath2); i++){
      if (backPath2[i] == '/'){
        temp[j] = '\0';
        strcat(backPath3, temp);
        strcat(backPath3, "/");
        _mkdir(backPath3);
        j = 0;
      }
      else
        temp[j++] = backPath2[i];
    }
    fptr = fopen(backPath2, "w");
    fclose(fptr);
  }
  closedir(directory);
}

/*
 * __reCreateDirectory__ recreates the directory just as it was commited in targetCommitID
 * calls __placeFile__ for help
 */
void reCreateDirectory(int targetCommitID){
  char path[MAX_FULLPATH_SIZE], temp[MAX_WIDE_FULLPATH_SIZE];
  DIR *mainDir, *dir2;
  struct dirent *entry;
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
    sprintf(temp, "%s/%s", path, entry->d_name);
    dir2 = opendir(temp);
    placeFile(dir2, temp);                                                                                   // make directories and place the file in its path
  }
  closedir(mainDir);
}

/*
 * __deleteFilesRecursively__ recursively deletes all files and directories in
 * the main project folder to get it ready for the reset operation
 */
void deleteFilesRecursively(DIR *directory, const char *backwardPath, int isMainPrjDir){
  struct dirent *entry;
  DIR *subDirectory;
  char fullFilePath[MAX_EXTRAWIDE_FULLPATH_SIZE];
  while((entry = readdir(directory)) != NULL){
    if (strcmp(entry->d_name, "_CTRLDIR") == 0)
      continue;
    if (strcmp(entry->d_name, ".") == 0)
      continue;
    if (strcmp(entry->d_name, "..") == 0)
      continue;
    sprintf(fullFilePath, "%s%s", backwardPath, entry->d_name);
    if ((subDirectory = opendir(fullFilePath)) != NULL){                                                     // it's a directory
      fullFilePath[strlen(fullFilePath)] = '/';
      deleteFilesRecursively(subDirectory, fullFilePath, 0);                                                 // work our way up recursively
    }
    else                                                                                                     // it's a file
      remove(fullFilePath);                                                                                  // delete the file
  }
  closedir(directory);
  if (isMainPrjDir == 0)
    rmdir(backwardPath);
}

/*
 * builds a temp file name that contains a given character and not the path
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

/*
 * __getChildID__ gets the child ID for the given commit ID
 * returns child ID, negative means it's a reset commit
 */
int getChildID(int cmtID){
  char path[MAX_FULLPATH_SIZE];
  int childID;
  sprintf(path, "_CTRLDIR/commit/commit%d/family.txt", cmtID);
  FILE *fptr = fopen(path, "r");
  fscanf(fptr, "%d", &childID);
  fclose(fptr);
  return childID;
}

/*
 * __createResetCommit__ creates a new commit with an updated ID, creates family.txt
 * inside it an inserts the negative of childID in it - this childID refers to the
 * commit this one was based on and reset from
 */
void createResetCommit(int targetCommitID){
  FILE *fptr;
  char tempPathName[MAX_FULLPATH_SIZE], path[MAX_FULLPATH_SIZE];
  fptr = fopen("_CTRLDIR/commit/commitID.txt", "w");
  fprintf(fptr, "%d", ++commitID);                                                                           // updating commit ID
  fclose(fptr);
  sprintf(path, "_CTRLDIR/commit/commit%d", commitID);
  _mkdir(path);
  sprintf(tempPathName, "%s/family.txt", path);
  fptr = fopen(tempPathName, "w");
  fprintf(fptr, "%d", -targetCommitID);                                                                      // setting up child ID (in this case it's negative)
  fclose(fptr);
}

/*
 * __deleteFiles__ recursively locates -f- files and removes them
 */
void deleteFiles(DIR *directory, const char *backwardPath){
  char temp[MAX_EXTRAWIDE_FULLPATH_SIZE], temp2[MAX_EXTRAWIDE_FULLPATH_SIZE], *fpPTR;
  char filePath[MAX_FULLPATH_SIZE];
  FILE *fptr;
  DIR *subDirectory;
  struct dirent *entry;
  while((entry = readdir(directory)) != NULL){
    if (strcmp(entry->d_name, ".") == 0)
      continue;
    if (strcmp(entry->d_name, "..") == 0)
      continue;
    sprintf(temp, "%s/%s", backwardPath, entry->d_name);
    if ((fptr = fopen(temp, "r")) != NULL){
      fclose(fptr);
      fpPTR = buildFileName(backwardPath, 'f');
      sprintf(temp2, "%s/%s", backwardPath, fpPTR);
      free(fpPTR);
      remove(temp2);
    }
    else{
      subDirectory = opendir(temp);
      deleteFiles(subDirectory, temp);
    }
  }
  closedir(directory);
}

/*
 * __deleteFullVersions__ opens folders and calls __deleteFiles__ to
 * delete the -f- version of all commited files in a target commit Folder
 */
void deleteFullVersions(int targetID){
  DIR *directory, *dir2;
  struct dirent *entry;
  char tempPathName[MAX_FULLPATH_SIZE], tempPath2[MAX_WIDE_FULLPATH_SIZE], *fpPTR;
  if (targetID > 1){                                                                                         // if target commit has full versions
    sprintf(tempPathName, "_CTRLDIR/commit/commit%d", targetID);
    directory = opendir(tempPathName);
    while((entry = readdir(directory)) != NULL){
      if (strcmp(entry->d_name, ".") == 0)
        continue;
      if (strcmp(entry->d_name, "..") == 0)
        continue;
      if (strcmp(entry->d_name, "family.txt") == 0)
        continue;
      if (strcmp(entry->d_name, "commitedFiles.txt") == 0)
        continue;
      sprintf(tempPath2, "%s/%s", tempPathName, entry->d_name);
      dir2 = opendir(tempPath2);
      deleteFiles(dir2, tempPath2);
    }
    closedir(directory);
  }
}

/*
 * __findMinID__ looks for the minimum origin ID among the files in target commit directory
 * inserts all the file paths and origin IDs into two other arrays
 * returns the minimum origin ID
 */
int findMinID(int *index, char pathsArchive[][MAX_FULLPATH_SIZE], int originsArchive[], FILE *filePointer){
  int minID = INT_MAX, originID, tmp, i;
  char c, tempPathName[MAX_FULLPATH_SIZE], tempPath[MAX_FULLPATH_SIZE];
  FILE *fptr;
  while(1){
    i = 0;
    while((c = fgetc(filePointer)) != EOF && c != '\n')
      tempPath[i++] = c;
    if (c == EOF)
      break;
    tempPath[i] = '\0';
    tmp = 0;
    fptr = fopen("_CTRLDIR/origin/_originFiles.txt", "r");
    while(1){
      i = 0;
      while((c = fgetc(fptr)) != EOF && c != '\n')
        tempPathName[i++] = c;
      if (c == EOF)
        break;
      tempPathName[i] = '\0';
      fscanf(fptr, "%d\n", &originID);
      if (strcmp(tempPathName, tempPath) == 0){
        tmp = 1;
        strcpy(pathsArchive[*index], tempPathName);
        originsArchive[(*index)++] = originID;
        break;
      }
    }
    if (originID < minID && tmp == 1)
      minID = originID;
    fclose(fptr);
  }
  fclose(filePointer);
  return minID;
}

/*
 * __createResetPath__ creates a path from current commitID to targetCommitID
 * by inserting all the commit IDs on the reset path into an array
 * returns length of the path array
 */
int createResetPath(int pathArr[], int minID, int targetCommitID){
  int index = 0, currentID = targetCommitID, childID;
  pathArr[index++] = currentID;
  if (minID == targetCommitID)
    return index;
  while(1){
    childID = getChildID(currentID);
    pathArr[index++] = childID;
    currentID = childID;
    if (childID < 0)
      currentID = -currentID;
    if (currentID == minID)
      break;
  }
  return index;
}

/*
 * __findInArchive__ takes a commit ID as an argument and looks for all file paths
 * whose origin IDs are the same as the given commit ID and then inserts these file
 * paths into a 2D array of characters
 * returns a 2D array of chars (i.e. an array of strings / a double pointer to chars)
 */
char **findInArchive(int cmtID, int length, const char pathsArchive[][MAX_FULLPATH_SIZE], const int originsArchive[]){
  char **arr;
  int i, index = 0;
  arr = malloc(length*sizeof(char*));
  for(i = 0; i < length; i++){
    if (originsArchive[i] == cmtID){
      arr[index] = malloc(MAX_FULLPATH_SIZE*sizeof(char));
      strcpy(arr[index++], pathsArchive[i]);
    }
  }
  arr[index] = malloc(2);
  arr[index][0] = '\0';
  return arr;
}

/*
 * __copyOtoM__ copies the given file from origin to main directory
 */
void copyOtoM(const char *fullFilePath){
  FILE *fptr1, *fptr2;
  char c, path[MAX_FULLPATH_SIZE];
  sprintf(path, "_CTRLDIR/origin/%s", fullFilePath);
  fptr1 = fopen(path, "r");
  fptr2 = fopen(fullFilePath, "w");
  while((c = fgetc(fptr1)) != EOF)
    fputc(c, fptr2);
  fclose(fptr1);
  fclose(fptr2);
}

/*
 * __createFile__ uses incremental versions of files to create the new versions
 * it does its work by creating fullVrs.txt to for a temp copy of the older version
 * and then calls __cf__ to do the work and add the new file to the main directory
 */
void createFile(const char *fullFilePath, int targetID){
  FILE *fptr1, *fptr2;
  char c, path2[MAX_EXTRAWIDE_FULLPATH_SIZE], path3[MAX_EXTRAWIDE_FULLPATH_SIZE];
  fptr1 = fopen(fullFilePath, "r");
  fptr2 = fopen("_CTRLDIR/temps/fullVrs.txt", "w");
  while((c = fgetc(fptr1)) != EOF)
    fputc(c, fptr2);
  fclose(fptr1);
  fclose(fptr2);
  sprintf(path2, "_CTRLDIR/commit/commit%d/%s/%s", targetID, fullFilePath, buildFileName(fullFilePath, 'd'));
  sprintf(path3, "%s", fullFilePath);

  cf(path2, path3);
  remove("_CTRLDIR/temps/fullVrs.txt");
}

/*
 * __copyCtoM__ copies files from commit folders to main directory
 */
void copyCtoM(DIR *directory, const char *cmtPath, const char *backwardPath){
  char c, tempPath[MAX_WIDE_FULLPATH_SIZE], backPath2[MAX_WIDE_FULLPATH_SIZE];
  int count = 0, temp = 1;
  DIR *subDirectory;
  FILE *fptr1, *fptr2;
  struct dirent *entry;
  while ((entry = readdir(directory)) != NULL){
    if (strcmp(entry->d_name, ".") == 0){
      count++;
      continue;
    }
    if (strcmp(entry->d_name, "..") == 0){
      count++;
      continue;
    }
    count++;
    sprintf(tempPath, "%s/%s/%s", cmtPath, backwardPath, entry->d_name);
    if ((fptr1 = fopen(tempPath, "r")) != NULL){
      if (temp == 0){
        fptr2 = fopen(backwardPath, "w");
        while((c = fgetc(fptr1)) != EOF)
          fputc(c, fptr2);
        fclose(fptr2);
      }
      else
        temp = 0;
      fclose(fptr1);
    }
    else{
      sprintf(backPath2, "%s/%s", backwardPath, entry->d_name);
      subDirectory = opendir(tempPath);
      copyCtoM(subDirectory, cmtPath, backPath2);
    }
  }
  if (count == 2)
    copyOtoM(backwardPath);
  closedir(directory);
}

/*
 * __reset__ recreates the main project directory just like it was
 * in the target commit ID - obviously contains only the commited files -
 * there are two cases: 1- if target is the last or the second to last commit, for which cases
 * all we have to do is to copy all the files since the full version of the files
 * already exists. and 2- if target is any other commit, in which case we have to
 * start from the initial version of the files from _originFiles.txt and then move
 * our way up to the current level. also note that __reset__ adds a new commit that
 * works only as a bridge and isn't an actual commit
 * so basically you cannot reset to a commit that is a reset commit itself
 * when __reset__ is executed, all selected files will be unselected
 * returns 0 upon success, -1 if faced errors
 */
int reset(int targetCommitID){
  FILE *fptr, *fptr1, *fptr2;
  char path[MAX_FULLPATH_SIZE], tempPathName[MAX_FULLPATH_SIZE], backPath2[MAX_WIDE_FULLPATH_SIZE], *fpPTR, c, **targetPaths;
  char commitDesc[MAX_COMMITDESC_SIZE], pathsArchive[MAX_FILE_COUNT][MAX_FULLPATH_SIZE];
  int childID, result, prevCommitID, minID, pathLength, archiveLength = 0, *archLenPointer, selectFlag;
  int pathArr[MAX_RESETPATH_SIZE], originsArchive[MAX_FILE_COUNT], cmtID, index, i, temp, read;
  DIR *mainDir, *dir2;
  struct dirent *entry;

  fptr1 = fopen("_CTRLDIR/commit/commitID.txt", "r");
  fscanf(fptr1, "%d", &commitID);                                                                            // getting current commit ID
  prevCommitID = commitID;
  fclose(fptr1);
  if (targetCommitID > prevCommitID || targetCommitID <= 0){                                                 // the given commit ID for reset should not be bigger than current commit ID  or non-positive
    SetConsoleTextAttribute(console, RED);
    printf("ERR. invalid commit ID for reset\n");
    SetConsoleTextAttribute(console, DEFAULT);
    return -1;
  }

  childID = getChildID(commitID);
  result = getChildID(targetCommitID);
  if (result < 0){                                                                                           // target id is a reset commit, cannot reset to a reset commit
    printf("Reset failed. target commit ID is a RESET commit\n-->Reset from: commit#%d\n", -result);
    return -1;
  }
  fptr = fopen("_CTRLDIR/flags/selectFlag.txt", "r+");
  fscanf(fptr, "%d", &selectFlag);
  if (selectFlag == 1){
    fseek(fptr, ftell(fptr)-1, SEEK_SET);
    fputc('0', fptr);                                                                                        // turning off the select flag
    remove("_CTRLDIR/temps/selectedFiles.txt");                                                              // unselecting all the files
  }
  fclose(fptr);
  printf("\n>>> processing...\n\n");
  mainDir = opendir(".");
  deleteFilesRecursively(mainDir, "", 1);                                                                    // emptying the current project directory
  reCreateDirectory(targetCommitID);                                                                         // recreating the main project directory as in target commit

  if (targetCommitID == commitID-1 || targetCommitID == childID){                                            // if target is either the child commit or the current commit
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

    createResetCommit(targetCommitID);                                                                       // starting here, we're updating commit info
    sprintf(commitDesc, "Reset commit from commit#%d", targetCommitID);
    addToLog(commitDesc);                                                                                    // adding commit info to log.txt
    deleteFullVersions(prevCommitID-1);                                                                      // deleting -f- versions of files from mentioned commit IDs
    deleteFullVersions(prevCommitID);
  }
  else{
    sprintf(path, "_CTRLDIR/commit/commit%d/commitedFiles.txt", targetCommitID);
    fptr = fopen(path, "r");
    archLenPointer = &archiveLength;
    minID = findMinID(archLenPointer, pathsArchive, originsArchive, fptr);                                   // finding the minimum origin ID to create the path
    pathLength = createResetPath(pathArr, minID, targetCommitID);                                            // creating the reset path and file path arrays
    temp = 0, read = 1;
    while(1){                                                                                                // begin creating files from -d- versions
      if (read == pathLength+1)                                                                              // reaching the end of the path array
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
    createResetCommit(targetCommitID);                                                                       // starting here, we're updating commit info
    sprintf(commitDesc, "Reset commit from commit#%d", targetCommitID);
    addToLog(commitDesc);                                                                                    // adding commit info to log.txt
    if (childID > 0){                                                                                        // if previous commit wasnt a reset commit
      deleteFullVersions(prevCommitID);
      deleteFullVersions(prevCommitID-1);
    }
  }
  SetConsoleTextAttribute(console, GREEN);
  printf("successfully reset to commit %d\n", targetCommitID);
  SetConsoleTextAttribute(console, DEFAULT);
  return 0;
}