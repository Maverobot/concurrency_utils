#include <catch2/catch.hpp>

#include <triple_buffer.h>

#include "utils.h"

using realtime_utils::TripleBuffer;
using namespace std::chrono_literals;

template <typename T>
void store(TripleBuffer<T>& buf) {
  for (size_t i = 0; i < 100; i++) {
    buf.store(i);
    std::this_thread::sleep_for(1ms);
  }
}

TEST_CASE("triple_buffer_simple_store_load", "tripe_buffer") {
  auto vec = generate_random_integers(100, 0, 300);

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

  std::thread store_t(store<int>, std::ref(buffer));

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
