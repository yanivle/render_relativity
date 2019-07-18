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
    std::cout << "] " << int(progress * 100.0) << "% " << total_time << " tot: "
              << estimated_total << " left: " << estimated_remaining << "  \r";
    std::cout.flush();
  }

  void done() {
    std::cout << std::endl;
  }

private:
  int max;
  time_t start;
};

#endif