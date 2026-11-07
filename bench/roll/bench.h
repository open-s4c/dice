#ifndef DICE_ROLL_BENCH_H
#define DICE_ROLL_BENCH_H
#include <stdint.h>

struct bench_event {
    uint64_t value;
};

void roll_publish(struct bench_event *event);
void roll_publish_dynamic(struct bench_event *event);
#endif
