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
    const char* counter_str_golden = R"(Counters:
  test1                          150
  test2                          30,000,000
)";
    CHECK(COUNTERS_STR() == counter_str_golden);
}

TEST_CASE("Counters are sorted", "[COUNTERS]") {
    DEFINE_COUNTER(sorting_test100);
    DEFINE_COUNTER(sorting_test2);
    DEFINE_COUNTER(sorting_test30);
    DEFINE_COUNTER(sorting_test40);
    DEFINE_COUNTER(sorting_test50);
    DEFINE_COUNTER(sorting_test7);

    COUNTER_INC_BY(sorting_test40,  40);
    COUNTER_INC_BY(sorting_test30,  30);
    COUNTER_INC_BY(sorting_test7,   7);
    COUNTER_INC_BY(sorting_test100, 100);
    COUNTER_INC_BY(sorting_test2,   2);
    COUNTER_INC_BY(sorting_test50,  50);

    const char* counter_str_golden = R"(Counters:
  sorting_test2                  2
  sorting_test7                  7
  sorting_test30                 30
  sorting_test40                 40
  sorting_test50                 50
  sorting_test100                100
  test1                          150
  test2                          30,000,000
)";
    CHECK(COUNTERS_STR() == counter_str_golden);
}
