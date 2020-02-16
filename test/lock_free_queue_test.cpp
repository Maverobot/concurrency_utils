#include <catch2/catch.hpp>

#include <concurrency_utils/lock_free_queue.h>

#include "utils.h"

#include <iostream>
#include <thread>

using concurrency_utils::LockFreeQueue;

void push(LockFreeQueue<int>& queue, const std::vector<int>& values) {
  for (const auto& v : values) {
    queue.push(v);
  }
}

void pop(LockFreeQueue<int>& queue, const std::vector<int>& values) {
  int i = 0;
  for (;;) {
    if (auto v_ptr = queue.pop()) {
      REQUIRE(values.at(i) == *v_ptr);
      if (++i >= values.size()) {
        return;
      }
    }
  }
}

TEST_CASE("lock_free_queue_single_producer_single_consumer", "lock_free_queue") {
  const std::vector<int> values = generate_random_integers(200, 0, 300);
  LockFreeQueue<int> queue;

  REQUIRE(queue.pop() == nullptr);

  std::thread push_thread(push, std::ref(queue), std::cref(values));
  std::thread pop_thread(pop, std::ref(queue), std::cref(values));

  push_thread.join();
  pop_thread.join();
}
