#ifndef SINGLETON_H
#define SINGLETON_H

/*
Singleton class.
It is threadsafe (assuming the underlying type is threadsafe).
Calls destructor on program exit.

Usage example:
SINGLETON(int, MyIntSingleton);
MyIntSingleon.instance()++;
*/

#define SINGLETON(type, name)                                  \
  class name {                                                 \
   private:                                                    \
    name(); /* Disallow instantiation outside of the class. */ \
   public:                                                     \
    name(const name &) = delete;                               \
    name &operator=(const name &) = delete;                    \
    name(name &&) = delete;                                    \
    name &operator=(name &&) = delete;                         \
                                                               \
    static auto &instance() {                                  \
      static type instance_;                                   \
      return instance_;                                        \
    }                                                          \
  };

#endif
