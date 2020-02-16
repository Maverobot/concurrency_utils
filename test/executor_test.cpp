#include <catch2/catch.hpp>

#include <executor.h>

#include "utils.h"

TEST_CASE("executor", "executor") {
  const auto vec = generate_random_integers(100, 1, 300);
  // const std::vector<int> vec{0, 1, 2, 3, 4, 5, 6, 7};
  std::atomic<bool> all_checked{false};
  // Define a function which is too slow to run in a realtime loop
  auto verification = [&vec, &all_checked, i = 0](int data) mutable {
    REQUIRE(data == vec.at(i));
    i++;

    if (i == vec.size()) {
      all_checked = true;
    }
  };

  realtime_utils::Executor<int> exec(verification);
  using namespace std::chrono_literals;

  for (auto v : vec) {
    exec.execute(v);
    std::this_thread::sleep_for(10ms);
  }

  while (!all_checked)
    ;
}
