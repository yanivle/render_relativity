#ifndef PROGRESS_H
#define PROGRESS_H

class Progress {
public:
  Progress(int max): max(max) {
    start = time(0);
  }

  void update(int i) {
    time_t total_time = time(0) - start;
    time_t estimated_total = max * total_time / (i + 1);
    time_t estimated_remaining = estimated_total - total_time;
    float progress = float(i) / float(max);
    const int bar_width = 60;
    std::cout << "[";
    int pos = bar_width * progress;
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << "% " << timestr(total_time) << " tot: "
              << timestr(estimated_total) << " left: " << timestr(estimated_remaining) << "  \r";
    std::cout.flush();
  }

  void done() {
    std::cout << std::endl;
  }

private:
  std::string timestr(time_t time) {
    int minutes = time / 60;
    int seconds = time - 60 * minutes;
    return std::to_string(minutes) + ':' + std::to_string(seconds);
  }

  int max;
  time_t start;
};

#endif
