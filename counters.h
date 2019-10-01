/***
 * Efficient multi-threaded counters library.
 * Usage:
 * #include "counters.h"
 * DEFINE_COUNTER(counter_name);
 * 
 * void do_work() {
 *   COUNTER_INC(name);
 *   COUNTER_INC_BY(name, 100);
 * }
 * 
 * std::cout << COUNTERS_STR() << std::endl;
 ***/

#ifndef COUNTERS_H
#define COUNTERS_H

#define DEFINE_COUNTER(name) thread_local LocalCounter counter_ ## name (#name);
#define COUNTER_INC(name) ((counter_ ## name).Increment())
#define COUNTER_INC_BY(name, delta) ((counter_ ## name).IncrementBy(delta))
#define COUNTERS_STR()  (global_counter_set.str())
#define COUNTERS_LOCAL_VALUE(name)  ((counter_ ## name).value())
#define COUNTERS_GLOBAL_VALUE(name)  (global_counter_set.value(#name))

#include <atomic>
#include <functional>
#include <stack> 
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <iostream>
#include <locale>
#include <iomanip>
#include "logging.h"

typedef unsigned long CounterValueType;

// Non-threadsafe counter.
class LocalCounter {
public:
  LocalCounter(const std::string& name);
  ~LocalCounter();

  void Increment() {value_++;}
  void IncrementBy(CounterValueType delta) {value_ += delta;}

  std::string str() const {
    std::stringstream res;
    res << "Counter(" << name_ << "): " << value_;
    return res.str();      
  }

  CounterValueType value() const { return value_; }
  const std::string& name() const { return name_; }

private:
  const std::string name_;
  CounterValueType value_ = 0;
};

class GlobalCounterSet {
public:
  void Register(LocalCounter* counter) {
      std::lock_guard<std::mutex> guard(mutex);
      local_counters_[counter->name()].push_back(counter);
  }

  void Unregister(LocalCounter* counter) {
      std::lock_guard<std::mutex> guard(mutex);
      std::vector<LocalCounter*>& vec = local_counters_[counter->name()];
      auto it = std::find(vec.begin(), vec.end(), counter);

      CHECK(it != vec.end()) << "invalid counter - name = " << counter->name();

      std::swap(*it, vec.back());
      vec.pop_back();

      deleted_counters_[counter->name()] += counter->value();
  }

  CounterValueType value(const std::string& name) const {
    std::lock_guard<std::mutex> guard(mutex);
    return value_no_lock(name);
  }

  std::map<std::string, CounterValueType> counters() const {
    std::lock_guard<std::mutex> guard(mutex);
    return counters_no_lock();
  }

  std::string str() const {
    std::lock_guard<std::mutex> guard(mutex);
    std::stringstream res;
    res << "Counters:" << std::endl;
    for (const auto& [name, value]: counters_no_lock()) {
        res.imbue(std::locale(""));
        res << "  " << std::left << std::setw(20) << name << " " << std::fixed << value << std::endl;
    }
    return res.str();      
  }

private:
  CounterValueType value_no_lock(const std::string& name) const {
    const std::vector<LocalCounter*>& vec = local_counters_.at(name);

    CounterValueType value = 0;
    const auto& it = deleted_counters_.find(name);
    if (it != deleted_counters_.end()) {
        value = it->second;
    }
    for (const auto& counter : vec) {
        value += counter->value();
    }
    return value;
  }

  std::map<std::string, CounterValueType> counters_no_lock() const {
    std::map<std::string, CounterValueType> res;
    for (const auto& [name, counters]: local_counters_) {
        res[name] = value_no_lock(name);
    }
    return res;
  }

  mutable std::mutex mutex;
  std::map<std::string, std::vector<LocalCounter*>> local_counters_;
  std::map<std::string, CounterValueType> deleted_counters_;
};

extern GlobalCounterSet global_counter_set;

#endif