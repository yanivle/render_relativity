#include <iostream>
#include <thread>
#include <vector>

#include "../counters.h"

#include "catch.hpp"

TEST_CASE("Single threaded counters work", "[COUNTERS]") {
    DEFINE_COUNTER(test1);

    COUNTER_INC_BY(test1, 100);
    for (int i = 0; i < 50; ++i) {
        COUNTER_INC(test1);
    }

    unsigned long local_val = COUNTERS_LOCAL_VALUE(test1);
    unsigned long global_val = COUNTERS_GLOBAL_VALUE(test1);
    CHECK(local_val == 150);
    CHECK(global_val == 150);
}

DEFINE_COUNTER(test2);

void increment_counter_thread(int amount) {
    for (int i = 0; i < amount; ++i) {
        COUNTER_INC(test2);
    }
}

TEST_CASE("Multi-threaded counters work", "[COUNTERS]") {
    std::vector<std::thread> threads;
    const unsigned long num_threads = 30;
    const unsigned long increment_amount = 1000000;
    const unsigned long final_counter_value = num_threads * increment_amount;
    for (int i = 0; i < num_threads; ++i) {
      threads.push_back(std::thread(increment_counter_thread, increment_amount));
    }
    for (std::thread& thread : threads) {
      thread.join();
    }

    unsigned long local_val = COUNTERS_LOCAL_VALUE(test2);
    unsigned long global_val = COUNTERS_GLOBAL_VALUE(test2);
    CHECK(local_val == 0);
    CHECK(global_val == final_counter_value);
    CHECK(COUNTERS_STR() == "Counter(test1): 150\nCounter(test2): 30000000\n");
}
