/* CIS 520 Proj 4
	Zach Arensman, Liam Bramley
*/

#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "sys/types.h"

#define UNUSED(x) (void)(x)

#define NUMLINES 1000000

typedef struct threadArgs{
	long start;
	long end;
}threadArgs;

int line_array[NUMLINES]; //hold ASCII values found

int num_threads = 1; //default 1, will be set by slurm

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

void* get_largest_ascii(void* threadArguments)
{
	FILE* fd;
	fd = fopen("/homes/dan/625/wiki_dump.txt", "r"); //Open file 

    size_t len = 0;
    char* line = NULL;

	long currentline = ((threadArgs*)threadArguments)->start; 
	
	long linestoRead = ((threadArgs*)threadArguments)->end - ((threadArgs*)threadArguments)->start;

	for(int i = 0; i < currentline; i++){ //Move to start position for thread
		getline(NULL,NULL,fd);
	}

	//Read numlines for this thread
	//printf("start: %ld, end %ld, lines to read %ld\n", ((threadArgs*)threadArguments)->start, ((threadArgs*)threadArguments)->end, linestoRead);
	for(int i = 0; i < linestoRead; i++)
	{ 
		getline(&line, &len, fd);
		line_array[currentline] = find_largest_ascii(line, len);
		currentline += 1;
  	}

	fclose(fd);

   return NULL;
}

void print_results()
{
 int i = 0;
  for ( i = 0; i < NUMLINES; i++ ) {
     printf("%d: %d\n", i, line_array[i]); //Print each line
  };
}


int main() {
	long i;
	num_threads = atoi(getenv("SLURM_CPUS_ON_NODE"));
	pthread_t threads[num_threads];
	pthread_attr_t attr;
	void *status;
	struct timeval t1, t2, t3, t4;
    double timeElapsedTotal;
    double timePthreads;
	double timePrint;

	long memusage;

	long startPos = 0;

	gettimeofday(&t1, NULL);

	double stepSize = (double)NUMLINES / (double)num_threads; //How many lines per thread

	printf("Starting on %s\n", getenv("HOSTNAME"));

	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	gettimeofday(&t2, NULL);

	threadArgs threadArguments[num_threads];

	for(int j = 0; j < num_threads; j++)
	{
		threadArguments[j].start = startPos + (stepSize * j);
		threadArguments[j].end = startPos + (stepSize * (j+1));
	}
	
	//Create threads
	for (i = 0; i < num_threads; i++ ) {
	    pthread_create(&threads[i], &attr, get_largest_ascii, (void*)&threadArguments[i]);
	    //get resource usage
  		struct rusage usage;
  		getrusage(RUSAGE_THREAD, &usage);
  		memusage = usage.ru_maxrss;
	}

	//Free attribute and wait for the other threads
	pthread_attr_destroy(&attr);
	for(i=0; i<num_threads; i++) {
	    pthread_join(threads[i], &status);
	}

	gettimeofday(&t3, NULL); 

	print_results();

	gettimeofday(&t4, NULL); 

	
	timePthreads = (t3.tv_sec - t2.tv_sec) * 1000.0; //Converted to milliseconds
    timePthreads += (t3.tv_usec - t2.tv_usec) / 1000.0;

	timePrint = (t4.tv_sec - t3.tv_sec) * 1000.0; //Converted to milliseconds
    timePrint += (t4.tv_usec - t3.tv_usec) / 1000.0;

    timeElapsedTotal = (t4.tv_sec - t1.tv_sec) * 1000.0; //Converted to milliseconds
    timeElapsedTotal += (t4.tv_usec - t1.tv_usec) / 1000.0;
	
	printf("\nCores / Threads: %d\n\n", num_threads);
	printf("Maximum Memory Usage(kB): %ld\n", memusage);
	printf("Maximum Memory Usage Per Thread(kB): %ld\n", (memusage / num_threads));
	printf("\nTime Pthreads: %fms\n", timePthreads);
	printf("Time Print: %fms\n", timePrint);
	printf("Total Time Elapsed: %fms\n", timeElapsedTotal);
	printf("\nMain: program completed. Exiting.\n");

	pthread_exit(NULL);
}

