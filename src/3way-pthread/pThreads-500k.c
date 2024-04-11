/* Finds the largest value of an ASCII character in a line
    pThreads
    Project 4
    Created on: 4/5/2024
    Author: Team 8
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>
#include "sys/types.h"


#define ARRAY_SIZE 500000
#define STRING_SIZE 2001
#define ALPHABET_SIZE 26

pthread_mutex_t mutexsum;
int num_threads = 2;
char line_largest[ARRAY_SIZE];
char lines[ARRAY_SIZE][STRING_SIZE];
FILE *file;

int parseLine(char *line) {
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9') p++;
	line[i - 3] = '\0';
	i = atoi(p);
	return i;
}

void readFile()
{
	int err,i;
	file = fopen( "/homes/dan/625/wiki_dump.txt", "r" );
	for ( i = 0; i < ARRAY_SIZE; i++ )  {
      err = fscanf( file, "%[^\n]\n", lines[i]);
      if( err == EOF ) break;
	}
	fclose( file );
}

char find_largest_char(char* line, int nchars) {
   int i, j;
   char *largest_char = NULL;

   for ( i = 0; i < nchars; i++ ) {
      if(line[i] > largest_char){
        largest_char = line[i];
      }
   }

   return largest_char;
}

void *count_array(int myID)
{
    int i, startPos, endPos;
	int nchars;

	startPos = myID * (ARRAY_SIZE / num_threads);
	endPos = startPos + (ARRAY_SIZE / num_threads);

	printf("myID = %d startPos = %d endPos = %d \n", myID, startPos, endPos);
	
	for(i = startPos; i < endPos; i++)
	{
		nchars = strlen( lines[i] );
		line_largest[i] = find_largest_char(lines[i], nchars);
	} 

	pthread_exit(NULL);
}

void print_results(int line_largest[])
{
  int i = 0;
  for ( i = 0; i < ARRAY_SIZE; i++ ) {
	printf("%d: %d\n", i, (int)line_largest[i]);
  }
}

main() {

	int i, rc;
	num_threads = atoi(getenv("SLURM_NTASKS"));
	pthread_t threads[num_threads];
	pthread_attr_t attr;
	void *status;
	struct timeval t1, t2, t3;
    double timeElapsedTotal;
	
	pthread_mutex_init(&mutexsum, NULL);
	
	readFile();

	/* Timing analysis begins */
    gettimeofday(&t1, NULL);
	printf("DEBUG: starting time on %s\n", getenv("HOSTNAME"));
	
	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < num_threads; i++ ) {
	      rc = pthread_create(&threads[i], &attr, count_array, (void *)i);
	      if (rc) {
	        printf("ERROR; return code from pthread_create() is %d\n", rc);
		    exit(-1);
	      }
	}
	
	/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(i=0; i<num_threads; i++) {
	     rc = pthread_join(threads[i], &status);
	     if (rc) {
		   printf("ERROR; return code from pthread_join() is %d\n", rc);
		   exit(-1);
	     }
	}
	
	gettimeofday(&t2, NULL); 

	print_results((int)line_largest);

	gettimeofday(&t3, NULL); 


    //total program time
    timeElapsedTotal = (t3.tv_sec - t1.tv_sec) * 1000.0; //Converted to milliseconds
    timeElapsedTotal += (t3.tv_usec - t1.tv_usec) / 1000.0;

	pthread_mutex_destroy(&mutexsum);
	
	printf("Tasks: %s\n Total Elapsed Time: %fms\n", getenv("SLURM_NTASKS"), timeElapsedTotal);
	printf("DATA, %s,%f\n", getenv("SLURM_NTASKS"), timeElapsedTotal);
	printf("Main: program completed. Exiting.\n");
	
	pthread_exit(NULL);
}