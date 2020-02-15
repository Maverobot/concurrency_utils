#pragma once

#include <algorithm>
#include <random>
#include <vector>

inline std::vector<int> generate_random_integers(size_t number, int min, int max) {
  // First create an instance of an engine.
  std::random_device rnd_device;
  // Specify the engine and distribution.
  std::mt19937 mersenne_engine{rnd_device()};  // Generates random integers
  std::uniform_int_distribution<int> dist{min, max};

  auto gen = [&dist, &mersenne_engine]() { return dist(mersenne_engine); };

  std::vector<int> vec(number);
  std::generate(begin(vec), end(vec), gen);
  return vec;
}
