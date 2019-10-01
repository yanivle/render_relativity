#include "counters.h"

GlobalCounterSet global_counter_set;

LocalCounter::LocalCounter(const std::string& name) : name_(name) {
    global_counter_set.Register(this);
}

LocalCounter::~LocalCounter() {
    global_counter_set.Unregister(this);
}
