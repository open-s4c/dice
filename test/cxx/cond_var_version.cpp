/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <cassert>
#include <cstdio>

extern "C" {
#include <dice/chains/capture.h>
#include <dice/chains/intercept.h>
#include <dice/events/malloc.h>
#include <dice/module.h>
#include <dice/pubsub.h>

int malloc_called;
int free_called;
int malloc_intercepted;
int free_intercepted;
int run_called;

PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_MALLOC, { malloc_called++; })
PS_SUBSCRIBE(CAPTURE_BEFORE, EVENT_FREE, { free_called++; })
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_MALLOC, { malloc_intercepted++; })
PS_SUBSCRIBE(INTERCEPT_BEFORE, EVENT_FREE, { free_intercepted++; })

}

class SyncObject {
private:
    std::condition_variable cv;
    std::mutex mtx;
    bool ready = false;
    uint64_t* buffer;

public:
    void waitForSignal() {
        std::unique_lock<std::mutex> lock(mtx);
        std::cout << "Thread 1 is waiting...\n";
        (*buffer)++;
        cv.wait(lock, [this]() { return ready; });
        std::cout << "Thread 1 received the signal!\n";
    }

    void sendSignal() {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
        (*buffer)--;
        std::cout << "Thread 2 is sending the signal.\n";
        cv.notify_one();
    }

    SyncObject() {
        buffer = (uint64_t *) malloc(sizeof(uint64_t));
    }

    ~SyncObject() {
        free(buffer);
    }
};


thread_local SyncObject bla_sync;

int main() {

    std::thread t2([]() {
        std::this_thread::sleep_for(std::chrono::seconds(1)); // give t1 time to wait
    });

    t2.join();

    printf("malloc_called %d\n", malloc_called);
    printf("free_called %d\n", malloc_called);
    printf("malloc_intercepted %d\n", malloc_intercepted);
    printf("free_intercepted %d\n", free_intercepted);
    assert(run_called == 1);
    assert(malloc_called == free_called);
    assert(malloc_intercepted == free_intercepted);
    return 0;
}
