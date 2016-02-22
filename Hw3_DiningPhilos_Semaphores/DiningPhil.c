#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>


int main(int argc, char *argv[])
{
    if(argc!=3)
    {
        printf("Error Msg: Invalid inputs - Expected ./Combiner <#Philosophers> <#iterations>\n");
        return 0;
    }
    const int NumPhilosophers = argv[1];
    const int NumIterations = argv[2];
    int 
    
    pid_t Pid_Status;

    int fd[2];
    int *status;
    int ret;
        
    while(Pid_Status > 0 && Philosopher_Id < NumPhilosophers)
    {
        Pid_Map = fork();	
    }

    if(Pid_Status > 0)
    {
    	// Wait for child processes
    	// close semaphores
    }
    else if(Pid_Status == 0)
    {

        ret = execlp ("./Philosopher", "./Philosopher", NumPhilosophers, NumIterations);

    }
    else if(Pid_Status== -1)
    {
        error("fork Philosopher");
    }
    else
    {
        ;
    }
    
    // if(Pid_Map > 0 && Pid_Redu > 0)
    // {   
    //     close(fd[0]);
    //     close(fd[1]);
    //     Pid_Status = wait(status);
    //     Pid_Status = wait(status);        
    // }
    return 0;
}
