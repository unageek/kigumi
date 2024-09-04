#pragma once

#include <algorithm>
#include <thread>

namespace kigumi {

class Global_options {
 public:
  static int num_threads() noexcept { return num_threads_; }

  static void set_num_threads(int num_threads) noexcept {
    num_threads_ = std::clamp(num_threads, 1,
                              std::max(1, static_cast<int>(std::thread::hardware_concurrency())));
  }

 private:
  static thread_local inline int num_threads_{
      std::max(1, static_cast<int>(std::thread::hardware_concurrency()))};
};

}  // namespace kigumi
