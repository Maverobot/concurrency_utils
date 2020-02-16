#include <catch2/catch.hpp>

#include <concurrency_utils/triple_buffer.h>

#include "utils.h"

using concurrency_utils::TripleBuffer;
using namespace std::chrono_literals;

const auto vec = generate_random_integers(100, 1, 300);

template <typename T, typename Rep, typename Period>
void store(TripleBuffer<T>& buf, const std::chrono::duration<Rep, Period>& rtime) {
  for (size_t i = 0; i < 100; i++) {
    buf.store(i);
    std::this_thread::sleep_for(rtime);
  }
}

TEST_CASE("triple_buffer_simple_store_load", "tripe_buffer") {
  TripleBuffer<int> buffer{vec.at(0)};
  REQUIRE(vec.at(0) == *buffer.wait_load_for(1ns));
  REQUIRE(vec.at(0) == buffer.instant_load());

  for (const auto& v : vec) {
    buffer.store(v);
    REQUIRE(v == *buffer.wait_load_for(1ns));
    REQUIRE(v == buffer.instant_load());
  }
}

TEST_CASE("triple_buffer_store_load_thread", "tripe_buffer") {
  TripleBuffer<int> buffer;

  std::thread store_t([&buffer] { store(buffer, 1ms); });

  SECTION("wait_load_for") {
    for (size_t i = 0; i < 100; i++) {
      auto val = buffer.wait_load_for(1s);
      REQUIRE(val.is_initialized());

      if (val) {
        REQUIRE(val.get() == i);
      }
    }
    store_t.join();
  }

  SECTION("wait_load_until") {
    for (size_t i = 0; i < 100; i++) {
      auto val = buffer.wait_load_until(std::chrono::steady_clock::now() + 1s);
      REQUIRE(val.is_initialized());

      if (val) {
        REQUIRE(val.get() == i);
      }
    }
    store_t.join();
  }
}

TEST_CASE("triple_buffer_empty_load", "triple_buffer") {
  TripleBuffer<int> buffer;
  REQUIRE_FALSE(buffer.wait_load_until(std::chrono::steady_clock::now() + 5ms).is_initialized());
  REQUIRE_FALSE(buffer.wait_load_for(5ms).is_initialized());
}

TEST_CASE("triple_buffer_stale", "triple_buffer") {
  const int initial = -1;
  TripleBuffer<int> buffer(initial);
  std::atomic<bool> go{false};
  std::thread store_t([&buffer, &go] {
    while (!go)
      ;
    for (const auto& v : vec) {
      buffer.store(v);
      std::this_thread::sleep_for(10ms);
    };
  });

  auto iter = vec.cbegin();
  go = true;
  for (;;) {
    auto v = buffer.instant_load();
    if (v == initial) {
      continue;
    }
    bool match = v == *iter || v == *(iter + 1);
    REQUIRE(match);
    if (v == *(iter + 1)) {
      iter++;
    }
    if (iter == vec.end() - 1) {
      break;
    }
  }

  store_t.join();
}
