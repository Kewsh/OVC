#include "main.h"

/*
 * __fileStatus__ checks whether a given file has been modified or recently added
 * __fileStatus__ creates a full version of a file from -d- versions if need be
 */
void fileStatus(const char *fullFilePath, int targetCommitID){
  int i, count = 0, tmp = 0, originID, pathArr[MAX_RESETPATH_SIZE], pathLength, cmtID, read, tmp2;
  char c1, c2, path[MAX_FULLPATH_SIZE], path2[MAX_EXTRAWIDE_FULLPATH_SIZE], path3[MAX_EXTRAWIDE_FULLPATH_SIZE];
  FILE *fptr1, *fptr2;
  for (i = 0; strlen(previousPaths[i]) != 0; i++){
    if (strcmp(fullFilePath, previousPaths[i]) == 0){
      count++;
      break;
    }
  }
  if (count == 0)                                                                                            // added file
    strcpy(addedFiles[addLen++], fullFilePath);
  else{                                                                                                      // checking whether the file has been modified or not
    sprintf(path, "_CTRLDIR/commit/commit%d/%s/%s", targetCommitID, fullFilePath, buildFileName(fullFilePath, 'f'));
    if ((fptr1 = fopen(path, "r")) != NULL){                                                                 // full version of the file is present
      fptr2 = fopen(fullFilePath, "r");
      while((c1 = fgetc(fptr1)) != EOF){
        c2 = fgetc(fptr2);
        if (c1 != c2 || c2 == EOF){
          tmp = 1;
          break;
        }
      }
      if (tmp == 1)                                                                                          // modified file
        strcpy(modifiedFiles[modLen++], fullFilePath);
      else{
        if ((c2 = fgetc(fptr2)) != EOF)                                                                      // modified file
          strcpy(modifiedFiles[modLen++], fullFilePath);
      }
      fclose(fptr1);
      fclose(fptr2);
    }
    else{                                                                                                    // there is no full version of the file in the commit folder
      tmp = 0;
      fptr1 = fopen("_CTRLDIR/origin/_originFiles.txt", "r");                                                // looking for the origin of the target file
      while(1){
        i = 0;
        while((c1 = fgetc(fptr1)) != '\n' && c1 != EOF)
          path[i++] = c1;
        if (c1 == EOF)
          break;
        path[i] = '\0';
        fscanf(fptr1, "%d\n", &originID);
        if (strcmp(path, fullFilePath) == 0){
          tmp = 1;
          break;
        }
      }
      fclose(fptr1);
      if (tmp == 1){
        if (originID == targetCommitID){                                                                     // target commit was the origin of the file
          sprintf(path, "_CTRLDIR/origin/%s", fullFilePath);
          tmp = 0;
          fptr1 = fopen(path, "r");
          fptr2 = fopen(fullFilePath, "r");
          while((c1 = fgetc(fptr1)) != EOF){
            c2 = fgetc(fptr2);
            if (c1 != c2 || c2 == EOF){
              tmp = 1;
              break;
            }
          }
          if (tmp == 1)                                                                                      // modified file
            strcpy(modifiedFiles[modLen++], fullFilePath);
          else{
            if ((c2 = fgetc(fptr2)) != EOF)                                                                  // modified file
              strcpy(modifiedFiles[modLen++], fullFilePath);
          }
          fclose(fptr1);
          fclose(fptr2);
        }
        else{                                                                                                // we need to create the full version of the file, using incrementals
          pathLength = createResetPath(pathArr, originID, targetCommitID);
          read = 1;
          tmp2 = 0;
          while(1){
            if (read == pathLength+1)                                                                        // reaching the end of the path array
              break;
            if (tmp2 == 1){                                                                                  // current commit is a reset commit, skip
              read++;
              tmp2 = 0;
              continue;
            }
            cmtID = pathArr[pathLength-read];
            if (cmtID < 0){
              cmtID = -cmtID;
              tmp2 = 1;
            }
            if (cmtID == originID){
              sprintf(path, "_CTRLDIR/origin/%s", fullFilePath);
              fptr1 = fopen(path, "r");
              fptr2 = fopen("_CTRLDIR/temps/fullVrs.txt", "w");
              while((c1 = fgetc(fptr1)) != EOF)
                fputc(c1, fptr2);
              fclose(fptr1);
              fclose(fptr2);
            }
            else{
              sprintf(path2, "_CTRLDIR/commit/commit%d/%s/%s", cmtID, fullFilePath, buildFileName(fullFilePath, 'd'));
              sprintf(path3, "_CTRLDIR/temps/fullVrs2.txt");

              cf(path2, path3);

              fptr1 = fopen("_CTRLDIR/temps/fullVrs2.txt", "r");
              fptr2 = fopen("_CTRLDIR/temps/fullVrs.txt", "w");
              while((c1 = fgetc(fptr1)) != EOF)
                fputc(c1, fptr2);
              fclose(fptr1);
              fclose(fptr2);
              remove("_CTRLDIR/temps/fullVrs2.txt");
            }
            read++;
          }
          tmp = 0;
          fptr1 = fopen("_CTRLDIR/temps/fullVrs.txt", "r");
          fptr2 = fopen(fullFilePath, "r");
          while((c1 = fgetc(fptr1)) != EOF){
            c2 = fgetc(fptr2);
            if (c1 != c2 || c2 == EOF){
              tmp = 1;
              break;
            }
          }
          if (tmp == 1)                                                                                      // modified file
            strcpy(modifiedFiles[modLen++], fullFilePath);
          else{
            if ((c2 = fgetc(fptr2)) != EOF)                                                                  // modified file
              strcpy(modifiedFiles[modLen++], fullFilePath);
          }
          fclose(fptr1);
          fclose(fptr2);
          remove("_CTRLDIR/temps/fullVrs.txt");
        }
      }
    }
  }
}

/*
 * __statusCheck__ recursively looks for each file in the project directory
 * and gives its full path to __fileStatus__ to check its status
 */
void statusCheck(DIR *directory, const char *backwardPath, int targetCommitID){
  char path[MAX_FULLPATH_SIZE];
  int templen;
  DIR *subDirectory;
  struct dirent *entry;
  FILE *fptr;
  while((entry = readdir(directory)) != NULL){
    if (strcmp(entry->d_name, ".") == 0)
      continue;
    if (strcmp(entry->d_name, "..") == 0)
      continue;
    if (strcmp(entry->d_name, "_CTRLDIR") == 0)
      continue;
    sprintf(path, "%s%s", backwardPath, entry->d_name);
    if ((fptr = fopen(path, "r")) != NULL){
      fclose(fptr);
      strcpy(currentPaths[curLen++], path);
      currentPaths[curLen][0] = '\0'; 
      fileStatus(path, targetCommitID);
    }
    else{
      templen = strlen(path);
      path[templen] = '/';
      path[templen+1] = '\0';
      subDirectory = opendir(path);
      statusCheck(subDirectory, path, targetCommitID);
    }
  }
  closedir(directory);
}

/*
 * __status__ finds the correct commit whose files are to be compared with the current files
 * calls __statusCheck__ to locate the files. finally, __status__ prints all the file states
 */
void status(void){
  char c, path[MAX_FULLPATH_SIZE];
  int i, j, k, childID, count, targetCommitID;
  addLen = 0, modLen = 0, prevLen = 0, curLen = 0;
  FILE *fptr;
  DIR *mainDir;
  struct dirent *entry;
  fptr = fopen("_CTRLDIR/commit/commitID.txt", "r");
  fscanf(fptr, "%d", &commitID);
  fclose(fptr);
  sprintf(path, "_CTRLDIR/commit/commit%d/family.txt", commitID);
  fptr = fopen(path, "r");
  fscanf(fptr, "%d", &childID);
  fclose(fptr);
  if (childID < 0)                                                                                           // last commit was a reset commit
    targetCommitID = -childID;                                                                               // opening the correct previous commit
  else
    targetCommitID = commitID;
  sprintf(path, "_CTRLDIR/commit/commit%d/commitedFiles.txt", targetCommitID);
  fptr = fopen(path, "r");
  while(1){
    k = 0;
    while((c = fgetc(fptr)) != '\n' && c != EOF)
      previousPaths[prevLen][k++] = c;                                                                       // array to hold the list of files in previous commit
    if (c == EOF)
      break;
    previousPaths[prevLen++][k] = '\0';
  }
  previousPaths[prevLen][0] = '\0';
  mainDir = opendir(".");
  statusCheck(mainDir, "", targetCommitID);
  printf("deleted files:\n");
  SetConsoleTextAttribute(console, RED);
  for (i = 0; i < prevLen; i++){
    count = 0;
    for (j = 0; strlen(currentPaths[j]) != 0; j++){
      if (strcmp(previousPaths[i], currentPaths[j]) == 0){
        count++;
        break;
      }
    }
    if (count == 0)
      printf("  ~%s\n", previousPaths[i]);
  }
  SetConsoleTextAttribute(console, DEFAULT);
  printf("-------------------------\n");
  printf("added files:\n");
  SetConsoleTextAttribute(console, GREEN);
  for (i = 0; i < addLen; i++)
    printf("  ~%s\n", addedFiles[i]);
  SetConsoleTextAttribute(console, DEFAULT);
  printf("-------------------------\n");
  printf("modified files:\n");
  for (i = 0; i < modLen; i++)
    printf("  ~%s\n", modifiedFiles[i]);
  printf("-------------------------\n");
}