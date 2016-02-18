#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX_LINE_SIZE   20
#define MAX_DEPTH_LIST  50
#define SIZE_OF_BUFF	20
#define NOT_FOUND       0
#define FOUND           1




extern void *MapPool_Updater(void *);
extern void *Mapper (void *);
extern void *Reducer (void *);
extern void *WC_writer(void *);

extern void CheckString_and_Add(char*);
extern void Print_PrevWords(void);

extern pthread_mutex_t mutexMP, mutexRP, mutexSP;
extern int inMP, inRP, inSP, outMP, outRP, outSP;
extern int numMP, numRP, numSP;
extern int N;
extern char *NewLine_Char;
//extern char MapperPool[MAX_LINE_SIZE][SIZE_OF_BUFF];
// extern char ReducerPool[MAX_LINE_SIZE][SIZE_OF_BUFF];
// extern char SummarizerPool[MAX_LINE_SIZE][SIZE_OF_BUFF];