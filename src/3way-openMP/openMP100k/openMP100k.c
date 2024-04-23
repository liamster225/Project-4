/* CIS 520 Proj 4
	Zach Arensman, Liam Bramley, Alex Bingham
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
#include <string.h>
#include <sys/resource.h>
#include "sys/types.h"
#include "sys/sysinfo.h"

#define NUMLINES 100000

int line_array[NUMLINES]; // holds the ASCII values found

typedef struct processMem_t{
	int virtualMem;
	int physicalMem;
} processMem_t;

int parseLine(char *line) {
    int i = strlen(line);
    const char *p = line;
    while (*p < '0' || *p > '9') p++;
    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}

//Code Example From: https://gist.github.com/gbmhunter/00c57b55e2616cd8e1f21f77b79e59fc?permalink_comment_id=2707469
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

int find_largest_ascii(char *line, int length) {
    int max_val = 0;
    for (int j = 0; j < length; j++) {
        if ((int)line[j] > max_val) {
            max_val = (int)line[j];
            if (max_val == 126) {
                return max_val;
            }
        }
    }
    return max_val;
}

void get_largest_ascii(int start, int end) {
    FILE *fd;
    fd = fopen("/homes/dan/625/wiki_dump.txt", "r"); // opens file

    size_t len = 0;
    char *line = NULL;

    int currentline = start;

    int linestoRead = end - start;

    for(int i = 0; i < currentline; i++){ //Move to start position for thread
		getline(NULL,NULL,fd);
	}

	//Read numlines for this thread
	for(int i = 0; i < linestoRead; i++)
	{ 
		getline(&line, &len, fd);
		line_array[currentline] = find_largest_ascii(line, len);
		currentline += 1;
  	}

	fclose(fd);
}

void print_results() {
    for (int i = 0; i < NUMLINES; i++) {
        printf("%d: %d\n", i, line_array[i]); // prints each line number and its largest ASCII value
    }
}

int main() {
    int num_threads = atoi(getenv("SLURM_CPUS_ON_NODE"));
    int chunk_size = NUMLINES / num_threads;

    printf("Starting on %s\n", getenv("HOSTNAME"));

    struct timeval t1, t2, t3, t4;
    double timeElapsedTotal;
    double timeOpenMP;
	double timePrint;

    processMem_t myMem; 

    gettimeofday(&t1, NULL);
    
    gettimeofday(&t2, NULL);

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int start = tid * chunk_size;
        int end = (tid == num_threads - 1) ? NUMLINES : start + chunk_size;

        get_largest_ascii(start, end);
    }


    gettimeofday(&t3, NULL); 

	print_results();

	gettimeofday(&t4, NULL); 

	
	timeOpenMP = (t3.tv_sec - t2.tv_sec) * 1000.0; //Converted to milliseconds
    timeOpenMP += (t3.tv_usec - t2.tv_usec) / 1000.0;

	timePrint = (t4.tv_sec - t3.tv_sec) * 1000.0; //Converted to milliseconds
    timePrint += (t4.tv_usec - t3.tv_usec) / 1000.0;

    timeElapsedTotal = (t4.tv_sec - t1.tv_sec) * 1000.0; //Converted to milliseconds
    timeElapsedTotal += (t4.tv_usec - t1.tv_usec) / 1000.0;
	
	GetProcessMemory(&myMem);

	printf("\nCores / Threads: %d\n\n", num_threads);
	printf("Virtual Memory: %ukB\nPhysical Memory: %ukB\n", myMem.virtualMem, myMem.physicalMem);
	printf("\nTime OpenMP: %fms\n", timeOpenMP);
	printf("Time Print: %fms\n", timePrint);
	printf("Total Time Elapsed: %fms\n", timeElapsedTotal);
	printf("\nMain: program completed. Exiting.\n");

    return 0;
}
