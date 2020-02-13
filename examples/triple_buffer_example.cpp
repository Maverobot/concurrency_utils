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

#include <triple_buffer.h>
#include <chrono>
#include <iostream>

using namespace std::chrono_literals;
using realtime_utils::TripleBuffer;

int main(int argc, char* argv[]) {
  TripleBuffer<double> buf;
  std::atomic<bool> stop{false};

  std::thread print_thread([&buf, &stop] {
    while (!stop) {
      std::cout << "instant value: " << buf << std::endl;
      if (auto val = buf.wait_load_for(100ms)) {
        std::cout << "waited value: " << val.get() << std::endl;
      }
    }
  });

  for (size_t i = 0; i < 10; i++) {
    buf = i;
    std::this_thread::sleep_for(200ms);
  }

  std::this_thread::sleep_for(1s);
  stop = true;
  print_thread.join();
  return 0;
}
