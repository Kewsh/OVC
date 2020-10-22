//headers

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <sys/stat.h>
#include <windows.h>
#include <wincon.h>

//color macros

#define GREEN 2 | 0 | FOREGROUND_INTENSITY
#define RED 4 | 0 | FOREGROUND_INTENSITY
#define PURPLE 5 | 0 | FOREGROUND_INTENSITY
#define DEFAULT FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE

//constants

#define MAX_FILENAME_SIZE 32
#define MAX_FULLPATH_SIZE 128
#define MAX_COMMITDESC_SIZE 128
#define MAX_RESETPATH_SIZE 128
#define MAX_CMDSTR_SIZE 256
#define MAX_FILE_COUNT 256
#define MAX_WIDE_FULLPATH_SIZE 256
#define MAX_EXTRAWIDE_FULLPATH_SIZE 512

//global variables

HANDLE console;
int commitID, addLen, modLen, prevLen, curLen;
char prjPath[MAX_FULLPATH_SIZE], sourcePath[MAX_FULLPATH_SIZE];
char addedFiles[MAX_FILE_COUNT][MAX_FULLPATH_SIZE], modifiedFiles[MAX_FILE_COUNT][MAX_FULLPATH_SIZE];
char previousPaths[MAX_FILE_COUNT][MAX_FULLPATH_SIZE], currentPaths[MAX_FILE_COUNT][MAX_FULLPATH_SIZE];

//function declarations

//stash
void copyBtoM(DIR *, const char *);
int stashPop(void);
void backupFiles(DIR *, const char *);
int stash(int);

//status
void fileStatus(const char *, int);
void statusCheck(DIR *, const char *, int);
void status(void);

//reset
void placeFile(DIR *, const char *);
void reCreateDirectory(int);
void deleteFilesRecursively(DIR *, const char *, int);
char *buildFileName(const char *, char);
int getChildID(int);
void createResetCommit(int);
void deleteFiles(DIR *, const char *);
void deleteFullVersions(int);
int findMinID(int *, char[][MAX_FULLPATH_SIZE], int[], FILE *);
int createResetPath(int[], int, int);
char **findInArchive(int, int, const char[][MAX_FULLPATH_SIZE], const int[]);
void copyOtoM(const char *);
void createFile(const char *, int);
void copyCtoM(DIR *, const char *, const char *);
int reset(int);

//unselect
int unselectFile(const char *);
void checkEmptiness(void);
void unselectAll(void);

//select
int selectFile(const char *);
void selectAllRecursive(DIR *, FILE *, const char *);
void selectAll(void);
void selectList(void);

//log
void printLog(void);
void addToLog(const char *);

//commit
void originate(const char *);
void launchDiff(const char *);
void addFile(const char *, int);
int checkFile(const char *);
int stageFiles(void);
int commit(const char *);

//init
void init(void);

//help
void help(const char *);