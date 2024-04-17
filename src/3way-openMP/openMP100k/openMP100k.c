#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUMLINES 100000

int line_array[NUMLINES]; // hold ASCII values found

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
        getline(&line, &len, fd);
        line_array[i] = find_largest_ascii(line, len);
    }

    fclose(fd);
}

void print_results() {
    for (int i = 0; i < NUMLINES; i++) {
        printf("%d: %d\n", i, line_array[i]); // prints each line
    }
}

int main() {
    int num_threads = atoi(getenv("SLURM_CPUS_ON_NODE"));
    int chunk_size = NUMLINES / num_threads;

    printf("Starting on %s\n", getenv("HOSTNAME"));

    #pragma omp parallel num_threads(num_threads)
    {
        int tid = omp_get_thread_num();
        int start = tid * chunk_size;
        int end = (tid == num_threads - 1) ? NUMLINES : start + chunk_size;

        get_largest_ascii(start, end);
    }

    print_results();
    printf("Main: program completed. Exiting.\n");

    return 0;
}
