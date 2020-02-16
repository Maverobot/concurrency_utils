/**
 * MIT License
 *
 * Copyright (c) 2020 Zheng Qu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <concurrency_utils/triple_buffer.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

namespace {
template <typename F, typename... Args>
struct is_invocable
    : std::is_constructible<std::function<void(Args...)>,
                            std::reference_wrapper<typename std::remove_reference<F>::type>> {};
}  // namespace

namespace realtime_utils {

template <typename DataT>
class Executor {
 public:
  Executor() = delete;

  template <typename F, typename = std::enable_if_t<is_invocable<F, DataT>::value>>
  Executor(F f) {
    keep_running_ = true;
    thread_ = std::thread(&Executor::execution_loop<F>, this, f);
  }

  /// Destructor
  ~Executor() {
    keep_running_ = false;
    while (is_running_) {
      std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  void execute(DataT data) { data_queue_.store(data); }

  void stop() { keep_running_ = false; }

 private:
  // non-copyable
  Executor(const Executor&) = delete;
  Executor& operator=(const Executor&) = delete;

  template <typename F>
  void execution_loop(F f) {
    is_running_ = true;
    while (keep_running_) {
      using namespace std::chrono_literals;
      if (auto value = data_queue_.wait_load_for(1s)) {
        f(*value);
      }
    }
    is_running_ = false;
  }

  std::thread thread_;

  TripleBuffer<DataT> data_queue_;

  std::atomic<bool> keep_running_;
  std::atomic<bool> is_running_;
};
}  // namespace realtime_utils
