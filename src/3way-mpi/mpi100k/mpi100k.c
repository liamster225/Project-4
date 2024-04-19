/* CIS 520 Proj 4
	Zach Arensman, Liam Bramley
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <mpi.h>
#include <stdint.h>
#include "sys/types.h"
#include "sys/sysinfo.h"
#include <math.h>
#include <sys/resource.h>

#define MAXIMUM_TASKS 32
#define STRING_SIZE 2001
#define NUMLINES 100000

unsigned int thread_locations[MAXIMUM_TASKS];
int line_array[NUMLINES]; //hold ASCII values found
int final_array[NUMLINES]; //hold ASCII values found
int num_threads = 1; //default 1, will be set by slurm

typedef struct threadArgs{
	long start;
	long end;
}threadArgs;

typedef struct {
	uint32_t virtualMem;
	uint32_t physicalMem;
} processMem_t;

void GetProcessMemory(processMem_t* processMem) {
	FILE *file = fopen("/proc/self/status", "r");
	char line[128];

	while (fgets(line, 128, file) != NULL) {
		if (strncmp(line, "VmSize:", 7) == 0) {
			processMem->virtualMem = parseLine(line);
		}

		if (strncmp(line, "VmRSS:", 6) == 0) {
			processMem->physicalMem = parseLine(line);
		}
	}

	fclose(file);
}

int find_largest_ascii(char* line, int length)
{
	 int max_val = 0;
	 for (int j = 0; j < length; j++ ) {
		if((int)line[j] > max_val) //If ascii char is new highest ascii val
		{
			max_val = (int)line[j]; //Make this the new max val
			if(max_val == 126)
			{
				return max_val;
			}
		}
	 }
	 return max_val; //return max ascii val
}

void* get_largest_ascii(int rank, FILE* fd)
{

	char tempBuffer[STRING_SIZE];
	int currentLine = rank * (NUMLINES/num_threads);

	fseek(fd, thread_locations[rank], SEEK_SET);
	
	/* While not at EOF and not beyond assigned section ... */
	while(currentLine < (rank+1) * (NUMLINES/num_threads)
		&& fscanf(fp, "%[^\n]\n", tempBuffer) != EOF)
	{
			/* Find and save average of line of char locally */
			int lineLength = strlen(tempBuffer);
			line_array[currentLine] = find_largest_ascii(tempBuffer, lineLength);
			currentLine++;
	}

   return NULL;
}

int parseLine(char *line) {
    int i = strlen(line);
    const char *p = line;
    while (*p < '0' || *p > '9') p++;
    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}

void print_results()
{
 int i = 0;
  for ( i = 0; i < NUMLINES; i++ ) {
     printf("%d: %d\n", i, final_array[i]); //Print each line
  };
}

int main(int argc, char *argv[])
{
    long i;
	num_threads = atoi(getenv("SLURM_CPUS_ON_NODE"));

	/* Time variables. */
	struct timeval t1, t2, t3, t4;
	double timeElapsedTotal;
	double timeMpi;
	double timePrint;
	
	int rc;
	int rank, numtasks;
    processMem_t myMem; 

	gettimeofday(&t1, NULL);

    FILE* fd;
	fd = fopen("/homes/dan/625/wiki_dump.txt", "r"); //Open file 

	/* MPI Setup. */
	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) 
	{
		printf ("Error starting MPI program. Terminating.\n");
	    MPI_Abort(MPI_COMM_WORLD, rc);
    }
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    /* Start performance */
    if(rank == 0)
    {
		gettimeofday(&t2, NULL);
		printf("DEBUG: starting time on %s\n", getenv("HOSTNAME"));
    }

	/* Distributes workload */
	if(rank == 0)
	{
		char tempBuffer[STRING_SIZE];
		i = 1;
		int currentLine = 0;

		/* Set starting position. */
		fseek(fd, 0, SEEK_SET);
		thread_locations[0] = ftell(fd);
		int prevPos = ftell(fd);

		/* Count lines and save their positions in file in the array. */
		while(currentLine < NUMLINES && fscanf(fd, "%[^\n]\n", tempBuffer) != EOF)
		{			
			if(currentLine == i * NUMLINES/num_threads)
			{
				thread_locations[i] = prevPos;
				i++;
			}
			prevPos = ftell(fd);
			currentLine++;
		}

		fseek(fd, 0, SEEK_SET);	
	}
	MPI_Bcast(thread_locations, num_threads, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

	get_largest_ascii(rank, fd);

	/* Merge local to global arrays */
	MPI_Reduce(line_array, final_array, NUMLINES, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

	/* Print results and record important data */
	if(rank == 0)
	{
		
		gettimeofday(&t3, NULL); 

		print_results();

		gettimeofday(&t4, NULL); 

		timeMpi = (t3.tv_sec - t2.tv_sec) * 1000.0; //Converted to milliseconds
    	timeMpi += (t3.tv_usec - t2.tv_usec) / 1000.0;

		timePrint = (t4.tv_sec - t3.tv_sec) * 1000.0; //Converted to milliseconds
    	timePrint += (t4.tv_usec - t3.tv_usec) / 1000.0;

    	timeElapsedTotal = (t4.tv_sec - t1.tv_sec) * 1000.0; //Converted to milliseconds
    	timeElapsedTotal += (t4.tv_usec - t1.tv_usec) / 1000.0;

			/* Performance metrics. */	
        GetProcessMemory(&myMem);
		printf("\nCores / Threads: %d\n\n", num_threads);
		printf("Virtual Memory: %ukB\nPhysical Memory: %ukB\n", myMem.virtualMem, myMem.physicalMem);
		printf("\nTime Pthreads: %fms\n", timeMpi);
		printf("Time Print: %fms\n", timePrint);
		printf("Total Time Elapsed: %fms\n", timeElapsedTotal);
		printf("\nMain: program completed. Exiting.\n");
	}
	fclose(fd);
	MPI_Finalize();
	return 0;
}