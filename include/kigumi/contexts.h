#pragma once

#include <algorithm>
#include <thread>

namespace kigumi {

class Num_threads {
 public:
  explicit Num_threads(std::size_t num_threads) {
    num_threads_ =
        std::clamp(num_threads, std::size_t{1},
                   static_cast<std::size_t>(std::max(1U, std::thread::hardware_concurrency())));
  }

  ~Num_threads() { num_threads_ = bak_; }

  Num_threads(const Num_threads&) = delete;
  Num_threads(Num_threads&&) = delete;
  Num_threads& operator=(const Num_threads&) = delete;
  Num_threads& operator=(Num_threads&&) = delete;

  static std::size_t current() { return num_threads_; }

 private:
  static thread_local inline std::size_t num_threads_{
      static_cast<std::size_t>(std::max(1U, std::thread::hardware_concurrency()))};

  std::size_t bak_{num_threads_};
};

}  // namespace kigumi
