#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>
//#include <sys/resource.h>

#define NUMLINES 100000

int line_array[NUMLINES]; // holds the ASCII values found

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

    fseek(fd, start, SEEK_SET); // moves to start position

    for (int i = start; i < end; i++) {
        fgets(line, len, fd);
        line_array[i] = find_largest_ascii(line, len);
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

    struct timeval t1, t2;
    double elapsedTime;

    gettimeofday(&t1, NULL);

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int start = tid * chunk_size;
        int end = (tid == num_threads - 1) ? NUMLINES : start + chunk_size;

        get_largest_ascii(start, end);
    }

    gettimeofday(&t2, NULL);
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; //Converted to milliseconds
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;

    print_results();
    printf("\nNumber of Threads: %d\n", num_threads);
    printf("Total Execution Time: %fms\n", elapsedTime);
    printf("Main: program completed. Exiting.\n");

    return 0;
}
