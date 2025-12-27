#include "../../src/core/JavaRandom.hpp"
#include <iostream>

int main9() {
  int64_t seed = 0;
  JavaRandom r(seed);

  std::cout << "=== C++ Chunk 0,0 Column 0 Simulation ===" << std::endl;

  r.nextDouble();
  r.nextDouble();
  r.nextDouble();

  for (int y = 127; y >= 0; y--) {
    int bedrockRandom = r.nextInt(5);

    if (y == 1) {
      std::cout << "Y=1: nextInt(5) = " + std::to_string(bedrockRandom)
                << std::endl;
    }
  }
  return 0;
}