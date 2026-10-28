#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "bench.h"

static uint64_t
now_(void)
{
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time))
        exit(EXIT_FAILURE);
    return (uint64_t)time.tv_sec * UINT64_C(1000000000) + time.tv_nsec;
}

int
main(int argc, char **argv)
{
    uint64_t iterations = 1000000;
    if (argc > 1) {
        char *end;
        errno      = 0;
        iterations = strtoull(argv[1], &end, 10);
        if (errno || !*argv[1] || *argv[1] == '-' || *end || !iterations)
            return EXIT_FAILURE;
    }
    if (argc > 3 ||
        (argc > 2 && strcmp(argv[2], "constant") && strcmp(argv[2], "dynamic")))
        return EXIT_FAILURE;
    bool dynamic             = argc > 2 && !strcmp(argv[2], "dynamic");
    struct bench_event event = {0};
    for (unsigned i = 0; i < 1000; i++)
        roll_publish(&event);
    if (event.value != 1000)
        return EXIT_FAILURE;
    event.value    = 0;
    uint64_t start = now_();
    if (dynamic) {
        for (uint64_t i = 0; i < iterations; i++)
            roll_publish_dynamic(&event);
    } else {
        for (uint64_t i = 0; i < iterations; i++)
            roll_publish(&event);
    }
    uint64_t elapsed = now_() - start;
    if (event.value != iterations)
        return EXIT_FAILURE;
    printf("%" PRIu64 ";%" PRIu64 ";%.3f;%" PRIu64 "\n", iterations, elapsed,
           (double)elapsed / iterations, event.value);
    return EXIT_SUCCESS;
}
