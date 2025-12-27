#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  int64_t worldSeed = -100L;
  JavaRandom random(worldSeed);

  PerlinNoise oct0(random); // Burn Octave 0
  PerlinNoise oct1(random); // Octave 1

  std::vector<double> noise(5 * 17 * 5, 0.0);

  double scale = 684.412 * 0.5;
  double amp = 0.5;

  oct1.populateNoiseArray(noise.data(), 0.0, 0.0, 0.0, 5, 17, 5, scale, scale,
                          scale, amp);

  std::cout << "=== C++ OCTAVE 1 GEN CHECK ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  std::cout << "idx 0: " << noise[0] << std::endl;
  std::cout << "idx 1: " << noise[1] << std::endl;

  return 0;
}