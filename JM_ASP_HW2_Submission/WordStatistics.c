#include "WordStats.h"

size_t line_size = MAX_LINE_SIZE;
int numMP = 0, numRP = 0, numSP = 0;
int inMP = 0, inRP = 0, inSP = 0, outMP = 0, outRP = 0, outSP = 0;
int N = SIZE_OF_BUFF;
int MP_Flag = 0, RP_Flag = 0, SP_Flag = 0;

pthread_mutex_t mutexMP, mutexRP, mutexSP;
pthread_cond_t fullMP,emptyMP, fullRP, emptyRP, fullSP, emptySP;

char *NewLine_Char = "\n";
char MapperPool[SIZE_OF_BUFF][SIZE_OF_BUFF][MAX_LINE_SIZE];
char ReducerPool[SIZE_OF_BUFF][SIZE_OF_BUFF][MAX_LINE_SIZE];
char SummarizerPool[SIZE_OF_BUFF][SIZE_OF_BUFF][MAX_LINE_SIZE];

char NextLine[MAX_LINE_SIZE];
char *NextWord;
char Word_List[MAX_DEPTH_LIST][MAX_LINE_SIZE];

int main(int argc, char *argv[])
{
    if(argc!=4)
    {
        printf("Error Msg: Invalid inputs - Expected ./Combiner <Input_filename.txt><#MapperThreads><#ReducerThreads>\n");
        return 0;
    }
    
    int Num_MapperThreads, Num_ReducerThreads;
    int rc;
    int Total_Num_Threads;
    int i;
    long tid;
    char *InputFileName;
    void* res[20];
    pthread_t threads[20];
    pthread_attr_t attr;

    InputFileName = argv[1];
    Num_MapperThreads = atoi(argv[2]);
    Num_ReducerThreads = atoi(argv[3]);

    pthread_mutex_init(&mutexMP,NULL);
    pthread_cond_init(&fullMP, NULL);
    pthread_cond_init(&emptyMP,NULL);

    pthread_mutex_init(&mutexRP,NULL);
    pthread_cond_init(&fullRP, NULL);
    pthread_cond_init(&emptyRP,NULL);

    pthread_mutex_init(&mutexSP,NULL);
    pthread_cond_init(&fullSP, NULL);
    pthread_cond_init(&emptySP,NULL);

    rc = pthread_create(&threads[0], NULL, MapPool_Updater, (void *)InputFileName);
    for (i = 1; i<=Num_MapperThreads; i++)
    {
    	rc = pthread_create(&threads[i], NULL, Mapper, (void *)InputFileName);
    }
    for (i = Num_ReducerThreads+1; i<=Num_MapperThreads+Num_ReducerThreads; i++)
    {
    	rc = pthread_create(&threads[i], NULL, Reducer, (void *)InputFileName);
    }
   	rc = pthread_create(&threads[Num_MapperThreads+Num_ReducerThreads+1], NULL, WC_writer, (void *)InputFileName);
    if (rc) 
    {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        //exit(-1);
    }


    pthread_join(threads[0], &res[0]);
    for (i = 1; i<=Num_MapperThreads; i++)
    	pthread_join(threads[i], &res[i]);
	for (i = Num_ReducerThreads+1; i<=Num_MapperThreads+Num_ReducerThreads; i++)
    	pthread_join(threads[i], &res[i]);
    pthread_join(threads[Num_MapperThreads+Num_ReducerThreads+1], &res[Num_MapperThreads+Num_ReducerThreads+1]);
    printf("Exiting Main thread\n");
    pthread_exit(NULL);
 }


void *MapPool_Updater(void *arg)
{
	char *NextLine_m;
    char *NewLine_Char = "\n";
    // char *
    char Prev_StartChar;
    int Current_Entry = 0;
    FILE *FileDesc;
    //struct MPUpdate *MPUpdat_stptr = (struct MPUpdate *)arg;
    char *FileName_m = (char *)arg/*MPUpdat_stptr->InputFileName*/;
    
    FileDesc = fopen(FileName_m,"r");

    

    if (FileDesc == NULL)
    {
        printf("Error Opening Input File in MapperUpdater");
        return 0;
    }
    
    while(getline(&NextLine_m, &line_size , FileDesc) != -1)
    {
        if (!strcmp(NextLine_m, NewLine_Char))
            continue;
       
         strtok(NextLine_m,"\n");

        if(Prev_StartChar != NextLine_m[0])
        {
        	
        	pthread_mutex_lock(&mutexMP);

	  		while (numMP == N) 
	  		{
	     		pthread_cond_wait(&fullMP, &mutexMP);
	  		}
	  		//printf("point1 \n");
        	strcpy(MapperPool[inMP][Current_Entry],"\0");
        	//printf("point2 \n");
        	Current_Entry = 0;

	        // MapperPool[inMP] = MPUpdate_st[inMP];

	        inMP = (inMP+1) % SIZE_OF_BUFF;
	        numMP++;
			Prev_StartChar = NextLine_m[0];
	        if(numMP == 1)
	        	pthread_cond_broadcast(&emptyMP);
			
	        pthread_mutex_unlock(&mutexMP);
        }

        pthread_mutex_lock(&mutexMP);

  		while (numMP == N) 
  		{
     		pthread_cond_wait(&fullMP, &mutexMP);
  		}

		strcpy(MapperPool[inMP][Current_Entry],NextLine_m);
		//printf("%s \n",MapperPool[inMP][Current_Entry] );
		Current_Entry++;
		

        if(numMP == 1)
        	pthread_cond_broadcast(&emptyMP);
		
        pthread_mutex_unlock(&mutexMP);

    }

	pthread_mutex_lock(&mutexMP);

	while (numMP == N) 
	{
 		pthread_cond_wait(&fullMP, &mutexMP);
	}

    strcpy(MapperPool[inMP][Current_Entry],"\0");

    inMP = (inMP+1) % SIZE_OF_BUFF;
    numMP++;
	
    if(numMP == 1)
    	pthread_cond_broadcast(&emptyMP);
	
	pthread_mutex_unlock(&mutexMP);


	pthread_mutex_lock(&mutexMP);

    MP_Flag = 1;
    pthread_cond_broadcast(&emptyMP);
	
    pthread_mutex_unlock(&mutexMP);

    fclose(FileDesc);
    printf("Exiting Mapper Updater\n");
    pthread_exit(NULL);
}


void *Mapper (void *threadID)
{
    char *NextLine_M; 
    char *item;
    int Current_MP_Entry = 0, Current_RP_Entry = 0;;
    char local_list[MAX_DEPTH_LIST][MAX_LINE_SIZE];
    int a = 5;
        
    while(1)
    {
        pthread_mutex_lock(&mutexMP);
        
        while (numMP == 0&&MP_Flag == 0) 
        {
        	pthread_cond_wait(&emptyMP, &mutexMP);
        }

        if(!(MP_Flag == 1 && numMP == 0))
        {
	        
	        Current_MP_Entry = 0;

	        while(1)
	        {

		        strcpy(local_list[Current_MP_Entry], MapperPool[outMP][Current_MP_Entry]);
		        Current_MP_Entry++;
		        if(!strcmp(MapperPool[outMP][Current_MP_Entry], "\0"))
		        	break;
		    }

	        outMP = (outMP + 1) % N;
	        numMP--; 

            if (numMP == N -1)
            	pthread_cond_broadcast(&fullMP);
		    pthread_mutex_unlock(&mutexMP);

	    	pthread_mutex_lock(&mutexRP);

	        while (numRP == N) 
	        {
	            pthread_cond_wait(&fullRP, &mutexRP);
	        }

		    for(Current_RP_Entry = 0; Current_RP_Entry<Current_MP_Entry; Current_RP_Entry++)
		    {
		        if(strcmp(local_list[Current_RP_Entry], "\0"))
		        	sprintf(ReducerPool[inRP][Current_RP_Entry],"(%s, 1)\n",local_list[Current_RP_Entry]);
		        else
		        	strcpy(ReducerPool[inRP][Current_RP_Entry],"\0");
		         // printf("%s \n", ReducerPool[inRP][Current_RP_Entry]);
		        	a= (a+1)*MAX_DEPTH_LIST%SIZE_OF_BUFF;
		    }
		    //printf("EndOfIt \n");

	        inRP = (inRP +1) % SIZE_OF_BUFF;
	        numRP++;

	        if(numRP == 1)
            	pthread_cond_broadcast(&emptyRP);

        	pthread_mutex_unlock(&mutexRP);
		}

		else if(MP_Flag == 1 && numMP == 0)
        {
        	pthread_mutex_lock(&mutexRP);
        	RP_Flag = 1;
        	pthread_cond_broadcast(&emptyRP);
        	pthread_mutex_unlock(&mutexRP);
        	pthread_mutex_unlock(&mutexMP);
        	
            break;
        }
    } 

    printf("Exiting Mapper THread \n");
    pthread_exit(NULL); 
}


void *Reducer (void *threadID)
{
    char *NextLine_M; 
    char *item;
    char *Word;
    int Current_RP_Entry = 0, Current_SP_Entry = 0;;
    int i = 0;
    char local_list[MAX_DEPTH_LIST][MAX_LINE_SIZE];
    int Word_Count[MAX_DEPTH_LIST];
	int Curr_List_Index = 0;
	int Word_Found = NOT_FOUND;
        
    while(1)
    {
        pthread_mutex_lock(&mutexRP);
        
        while (numRP == 0&&RP_Flag == 0) 
        {
        	pthread_cond_wait(&emptyRP, &mutexRP);
        }

        if(!(RP_Flag == 1 && numRP == 0))
        {
	        
	        Current_RP_Entry = 0;
	        Curr_List_Index = 0;

	        while(1)
	        {
		        Word = ReducerPool[outRP][Current_RP_Entry];

		        if(!strcmp(ReducerPool[outRP][Current_RP_Entry], "\0"))
		        	break;

		        Word = strtok(strtok(Word,"("), ",");
		        	 //printf("%s\n", Word);
			    
			    Word_Found = NOT_FOUND;
			    for(i=0; i<Curr_List_Index; i++)
			    {
			        if (!strcmp(Word,local_list[i]))
			        {
			            Word_Count[i] = Word_Count[i]+1;
			            Word_Found = FOUND;
			            break;
			        }
			    }
			    if (Word_Found == NOT_FOUND)
			    {
			        strcpy(local_list[Curr_List_Index], Word);
			        Curr_List_Index++;
			    }

		        Current_RP_Entry++;
		        if(!strcmp(ReducerPool[outRP][Current_RP_Entry], "\0"))
		        	break;
		    }

	        outRP = (outRP + 1) % N;
	        numRP--; 

            if (numRP == N -1)
            	pthread_cond_broadcast(&fullRP);
		    pthread_mutex_unlock(&mutexRP);

	    	pthread_mutex_lock(&mutexSP);

	        while (numSP == N) 
	        {
	            pthread_cond_wait(&fullSP, &mutexSP);
	        }

		    for(Current_SP_Entry = 0; Current_SP_Entry<Curr_List_Index; Current_SP_Entry++)
		    {
		        if(strcmp(local_list[Current_SP_Entry], "\0"))
		        	sprintf(SummarizerPool[inSP][Current_SP_Entry],"(%s, %d)", local_list[Current_SP_Entry], Word_Count[Current_SP_Entry]+1);
		        else
		        	strcpy(SummarizerPool[inSP][Current_SP_Entry],"\0");
		        Word_Count[Current_SP_Entry] = 0;
		         //printf("%s \n",SummarizerPool[inSP][Current_SP_Entry]);
		    }

	        inSP = (inSP +1) % SIZE_OF_BUFF;
	        numSP++;

	        if(numSP == 1)
            	pthread_cond_broadcast(&emptySP);

        	pthread_mutex_unlock(&mutexSP);
		}

		else if(RP_Flag == 1 && numRP == 0)
        {
        	pthread_mutex_lock(&mutexSP);
        	SP_Flag = 1;
        	pthread_cond_broadcast(&emptySP);
        	pthread_mutex_unlock(&mutexSP);
        	pthread_mutex_unlock(&mutexRP);
        	
            break;
        }
    } 

    printf("Exiting Reducer THread \n");
    pthread_exit(NULL); 
}

void *WC_writer (void *threadID)
{
    char *NextLine_M; 
    char *item;
    int Current_SP_Entry = 0;
    char local_list[MAX_DEPTH_LIST][MAX_LINE_SIZE];
    int i = 0;
    FILE *FileDesc;
        
    FileDesc = fopen("wordCount.txt","w+");
    fclose(FileDesc);
    FileDesc = fopen("wordCount.txt","a");
    while(1)
    {
        pthread_mutex_lock(&mutexSP);
        
        while (numSP == 0&&SP_Flag == 0) 
        {
        	pthread_cond_wait(&emptySP, &mutexSP);
        }

        if(!(SP_Flag == 1 && numSP == 0))
        {
	        
	        Current_SP_Entry = 0;

	        while(1)
	        {

		        strcpy(local_list[Current_SP_Entry], SummarizerPool[outSP][Current_SP_Entry]);
		        Current_SP_Entry++;
		        if(!strcmp(SummarizerPool[outSP][Current_SP_Entry], "\0"))
		        	break;
		    }

	        outSP = (outSP + 1) % N;
	        numSP--; 

            if (numSP == N -1)
            	pthread_cond_broadcast(&fullSP);
		    pthread_mutex_unlock(&mutexSP);


		    for(i = 0; i<Current_SP_Entry; i++)
		    {
		        if(strcmp(local_list[i], "\0"))
		        {
		        	 ;
		        	fprintf(FileDesc,"%s\n",local_list[i]);
		        }
		        else
		        	;
		    }

		}

		else if(SP_Flag == 1 && numSP == 0)
        {
        	
        	pthread_mutex_unlock(&mutexSP);
        	
            break;
        }
    } 

    fclose(FileDesc);

    printf("Exiting WC_Writer THread \n");
    pthread_exit(NULL); 
}

