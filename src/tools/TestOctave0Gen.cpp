#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  int64_t worldSeed = -100L;
  JavaRandom random(worldSeed);
  PerlinNoise oct0(random);

  std::vector<double> noise(5 * 17 * 5, 0.0);

  oct0.populateNoiseArray(noise.data(), 0.0, 0.0, 0.0, 5, 17, 5, 684.412,
                          684.412, 684.412, 1.0);

  std::cout << "=== C++ OCTAVE 0 GEN CHECK ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  std::cout << "idx 0: " << noise[0] << std::endl;
  std::cout << "idx 1: " << noise[1] << std::endl;
  std::cout << "idx 2: " << noise[2] << std::endl;
  int midIdx = (0 * 5 + 0) * 17 + 8;
  std::cout << "idx " << midIdx << ": " << noise[midIdx] << std::endl;

  return 0;
}