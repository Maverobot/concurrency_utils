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
