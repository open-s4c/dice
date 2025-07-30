/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2025. All rights reserved.
 * SPDX-License-Identifier: MIT
 */

#include <iostream>
#include <thread>

class Object
{
  private:
    uint64_t *buffer;

  public:
    uint64_t increment()
    {
        return (*buffer)++;
    }

    Object()
    {
        buffer = (uint64_t *)malloc(sizeof(uint64_t));
    }

    ~Object()
    {
        free(buffer);
    }
};


thread_local Object something;

int
main()
{
    std::thread t2([]() {
        std::this_thread::sleep_for(
            std::chrono::seconds(1)); // give t1 time to wait
        std::cout << "Something: " << something.increment() << std::endl;
        std::cout << "Something: " << something.increment() << std::endl;
    });

    t2.join();

    return 0;
}
