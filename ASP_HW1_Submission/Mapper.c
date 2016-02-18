#include <stdio.h>
#include <string.h>

#define MAX_LINE_SIZE   20

int main (int argc, char *argv[])
{
    char NextLine[MAX_LINE_SIZE];
    char *NewLine_Char = "\n";
    FILE *FileDesc;
    char *FileName = argv[0];
    
    FileDesc = fopen(FileName,"r");

    if (FileDesc == NULL)
    {
        printf("Error Opening Input File in Mapper");
        return 0;
    }
    
    while(fgets(NextLine, MAX_LINE_SIZE, FileDesc) != NULL)
    {
        if (!strcmp(NextLine, NewLine_Char))
            continue;
        printf("(%s, 1)\n", strtok(NextLine,"\n"));
    }
   
    fclose(FileDesc);
    return 0;
}

