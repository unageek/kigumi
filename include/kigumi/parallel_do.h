#pragma once

#include <kigumi/contexts.h>

#include <algorithm>
#include <atomic>
#include <exception>
#include <iterator>
#include <mutex>
#include <thread>
#include <vector>

namespace kigumi {

template <class RandomAccessIterator, class State, class Body, class Post>
void parallel_do(RandomAccessIterator first, RandomAccessIterator last, State state, Body body,
                 Post post) {
  auto size = static_cast<std::size_t>(std::distance(first, last));
  if (size == 0) {
    return;
  }

  auto num_threads = std::min(Num_threads::current(), size);
  if (num_threads == 1) {
    for (auto it = first; it != last; ++it) {
      body(*it, state);
    }
    post(state);
    return;
  }

  std::vector<std::thread> threads;
  std::atomic<std::size_t> next_index{};
  std::mutex mutex;
  std::exception_ptr exception_ptr;

  threads.reserve(num_threads);
  for (std::size_t tid = 0; tid < num_threads; ++tid) {
    threads.emplace_back([&] {
      auto local_state = state;

      try {
        while (true) {
          auto index = next_index++;
          if (index >= size) {
            break;
          }

          body(*(first + index), local_state);

          if (exception_ptr) {
            return;
          }
        }

        std::lock_guard lock{mutex};
        post(local_state);
      } catch (...) {
        std::lock_guard lock{mutex};
        if (!exception_ptr) {
          exception_ptr = std::current_exception();
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  if (exception_ptr) {
    std::rethrow_exception(exception_ptr);
  }
}

template <class RandomAccessIterator, class Body>
void parallel_do(RandomAccessIterator first, RandomAccessIterator last, Body body) {
  auto size = static_cast<std::size_t>(std::distance(first, last));
  if (size == 0) {
    return;
  }

  auto num_threads = std::clamp(static_cast<std::size_t>(std::thread::hardware_concurrency()),
                                std::size_t{1}, size);
  if (num_threads == 1) {
    for (auto it = first; it != last; ++it) {
      body(*it);
    }
    return;
  }

  std::vector<std::thread> threads;
  std::atomic<std::size_t> next_index{};
  std::mutex mutex;
  std::exception_ptr exception_ptr;

  threads.reserve(num_threads);
  for (std::size_t tid = 0; tid < num_threads; ++tid) {
    threads.emplace_back([&] {
      try {
        while (true) {
          auto index = next_index++;
          if (index >= size) {
            break;
          }

          body(*(first + index));

          if (exception_ptr) {
            return;
          }
        }
      } catch (...) {
        std::lock_guard lock{mutex};
        if (!exception_ptr) {
          exception_ptr = std::current_exception();
        }
      }
    });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  if (exception_ptr) {
    std::rethrow_exception(exception_ptr);
  }
}

}  // namespace kigumi
