#include <catch2/catch.hpp>

#include <lock_free_queue.h>

#include "utils.h"

#include <iostream>
#include <thread>

#define REQUIRE_MESSAGE(cond, msg) \
  do {                             \
    INFO(msg);                     \
    REQUIRE(cond);                 \
  } while ((void)0, 0)

using realtime_utils::LockFreeQueue;

void push(LockFreeQueue<int>& queue, const std::vector<int>& values) {
  for (const auto& v : values) {
    queue.push(v);
  }
}

void pop_1(LockFreeQueue<int>& queue, const std::vector<int>& values) {
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

  std::thread push_thread(push, std::ref(queue), std::cref(values));
  std::thread pop_thread(pop_1, std::ref(queue), std::cref(values));

  push_thread.join();
  pop_thread.join();
}

void pop_2(LockFreeQueue<int>& queue,
           const std::vector<int>& values_1,
           const std::vector<int>& values_2) {
  int i = 0;
  int j = 0;
  for (;;) {
    if (auto v_ptr = queue.pop()) {
      bool well_received = values_1.at(i) == *v_ptr || values_2.at(i) == *v_ptr;
      REQUIRE(well_received);
      if (values_1.at(i) == *v_ptr) {
        i++;
      }
      if (values_2.at(j) == *v_ptr) {
        j++;
      }

      if (i >= values_1.size() && j >= values_2.size()) {
        return;
      }
    }
  }
}

TEST_CASE("lock_free_queue_multi_producer_single_consumer", "lock_free_queue") {
  const std::vector<int> values_1 = generate_random_integers(200, 0, 300);
  const std::vector<int> values_2 = generate_random_integers(200, 0, 300);
  LockFreeQueue<int> queue;

  std::thread push_thread_1(push, std::ref(queue), std::cref(values_1));
  std::thread push_thread_2(push, std::ref(queue), std::cref(values_2));
  std::thread pop_thread(pop_2, std::ref(queue), std::cref(values_1), std::cref(values_2));

  push_thread_1.join();
  push_thread_2.join();
  pop_thread.join();
}
