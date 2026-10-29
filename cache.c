#include <assert.h>
#include <stdio.h>
#include <time.h>

#define CACHE_LINE_SIZE 64
#define MAX_NODES 16
#define ITERATIONS 5000000

// Try different distances between memory locations.
#define NUM_STRIDES 4

long data[MAX_NODES * 4096];

double get_time(void)
{
    struct timespec tm;
    assert(clock_gettime(CLOCK_MONOTONIC, &tm) == 0);

    return (double)tm.tv_sec +
           (double)tm.tv_nsec / 1000000000.0;
}

void run_test(int nodes, int stride_bytes)
{
    int stride = stride_bytes / sizeof(long);

    // Link the selected memory locations in a cycle.
    for (int i = 0; i < nodes; i++) {
        data[i * stride] = (i + 1) % nodes;
    }

    volatile long index = 0;

    double t0 = get_time();

    for (long i = 0; i < ITERATIONS; i++) {
        index = data[index * stride];
    }

    double t1 = get_time();

    printf("Stride: %5d B | Locations: %2d | "
           "Time: %.4f s | %.2f ns/access\n",
           stride_bytes,
           nodes,
           t1 - t0,
           (t1 - t0) * 1e9 / ITERATIONS);
}

int main(void)
{
    int strides[NUM_STRIDES] = {
        4096, 8192, 16384, 32768
    };

    for (int s = 0; s < NUM_STRIDES; s++) {
        printf("\nTesting stride of %d bytes\n", strides[s]);

        for (int nodes = 1; nodes <= MAX_NODES; nodes++) {
            run_test(nodes, strides[s]);
        }
    }

    return 0;
}