#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

void handleCE(int);

//char *InputFileName;

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("Error Msg: Invalid inputs - Expected ./Combiner <Input_filename.txt>\n");
        return 0;
    }
    char *InputFileName;
    InputFileName = argv[1];
    
    pid_t Pid_Map, Pid_Redu, Pid_Status;
    int fd[2];
    int *status;
    int ret;
    
    pipe(fd);
    
    Pid_Map = fork();
    if(Pid_Map > 0)
    {
        Pid_Redu = fork();
        if(Pid_Redu == 0)
        {
            close(fd[1]);
            dup2(fd[0],0);
            ret = execlp ("./Reducer", InputFileName, (char *)0);
            close(fd[0]);
            close(0);
        }
        else if(Pid_Redu == -1)
        {
            error("fork Reducer");
        }
        else
        {
            ;
        }
    }
    else if(Pid_Map == 0)
    {
        close(fd[0]);
        dup2(fd[1],1);
        ret = execlp ("./Mapper", InputFileName, (char *)0);
    //ret = execlp ("./Mapper", InputFileName,InputFileName, (char *)0);
        close(fd[1]);
        close(1);
    }
    else if(Pid_Map == -1)
    {
        error("fork Mapper");
    }
    else
    {
        ;
    }
    
    if(Pid_Map > 0 && Pid_Redu > 0)
    {   
        close(fd[0]);
        close(fd[1]);
        Pid_Status = wait(status);
        Pid_Status = wait(status);        
    }
    return 0;
}

