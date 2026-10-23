#include <assert.h>
#include <stdio.h>
#include <time.h>

#define MAX_SZ 17 // 2^(17+3) = 1 MB
#define NN (1 << (MAX_SZ + 3))

long data[NN];

double
get_time()
{
    struct timespec tm;
    assert(0 == clock_gettime(CLOCK_MONOTONIC, &tm));
    double ts = (double)tm.tv_sec;
    ts += ((double)tm.tv_nsec) / (1000.0 * 1000.0 * 1000.0);
    return ts;
}

static signed long state = 88172645463325252ULL; // Must be non-zero

static inline long
roll(long xx, long sz)
{
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return (state + xx) & ((1 << sz) - 1);
}

void
run_size_test(long sz)
{
    long count = 100 * 1000 * 1000;
    long nn = 1 << sz;

    double t0 = get_time();

    long yy = 0;
    for (long jj = 0; jj < count; ++jj) {
        int xx = data[roll(yy, sz)];
        yy += xx;
    }

    double t1 = get_time();

    printf("sz = %ld (%ld B), time = %.03fs\n", sz, (1L << sz) * 8, t1 - t0);

    assert(yy == count);
}

int
main(int argc, char* argv[])
{
    // Fill the block with 1's.
    for (long ii = 0; ii < NN; ++ii) {
        data[ii] = 1;
    }

    for (long jj = 8; jj <= MAX_SZ; ++jj) {
        run_size_test(jj);
    }
}
