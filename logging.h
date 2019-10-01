#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <sstream>
#include <iostream>

#define CHECK(expr) (expr) ? 0 : logging::LoggerVoidifier() && logging::CheckLogger(__FILE__, __LINE__, __func__, #expr)

namespace logging {
class CheckLogger {
public:
  CheckLogger(const std::string& file, int line, const std::string& function,
              const std::string& expr) {
      ss_ << "Check failed " << expr << ": " << file << ":" << line << "@" << function << " ";
  }

  template<class T>
  CheckLogger& operator<< (const T& t) {
      ss_ << t;
      return *this;
  }

  ~CheckLogger() {
      std::cerr << ss_.str() << std::endl;
      abort();
  }

private:
  std::stringstream ss_;
};

class LoggerVoidifier {
public:
    int operator &&(const CheckLogger& logger) const { return 0; }
};

}

#endif
