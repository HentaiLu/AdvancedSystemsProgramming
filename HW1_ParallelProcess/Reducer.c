#include <stdio.h>
#include <string.h>

#define MAX_LINE_SIZE   20
#define MAX_DEPTH_LIST  50
#define NOT_FOUND       0
#define FOUND           1

void CheckString_and_Add(char*);
void Print_PrevWords(void);

char Word_List[MAX_DEPTH_LIST][MAX_LINE_SIZE];
char *NextWord;
int Word_Count[MAX_DEPTH_LIST];
int Curr_List_Index = 0;
size_t line_size = MAX_LINE_SIZE;

int main (int argc, char *argv[])
{
    char *NextLine;
    char *NewLine_Char = "\n";
    char *FileName = argv[0];
    char *OutWord;
    char Prev_StartChar;
    
    while(getline(&NextLine, &line_size , stdin) != -1)
    {        
        NextWord = strtok(strtok(NextLine,"("), ",");
        if ( Prev_StartChar != NextWord[0])
            Print_PrevWords();
        CheckString_and_Add(NextWord);
        Prev_StartChar = NextWord[0];
    }
    Print_PrevWords();
    return 0;
}

void CheckString_and_Add(char* Word)
{
    int i=0;
    int Word_Found = NOT_FOUND;
    
    for(i=0; i<Curr_List_Index; i++)
    {
        if (!strcmp(Word,Word_List[i]))
        {
            Word_Count[i] = Word_Count[i]+1;
            Word_Found = FOUND;
            break;
        }
    }
    if (Word_Found == NOT_FOUND)
    {
        strcpy(Word_List[Curr_List_Index], Word);
        Curr_List_Index++;
    }
}

void Print_PrevWords(void)
{
    int i = 0;
    for( i=0; i<Curr_List_Index; i++)
    {
        printf("(%s, %d)\n", Word_List[i], Word_Count[i]+1);
        Word_Count[i]=0;
    }
    Curr_List_Index = 0;
}




