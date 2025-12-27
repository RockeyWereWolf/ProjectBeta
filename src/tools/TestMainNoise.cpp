#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  int64_t worldSeed = -100L;
  JavaRandom random(worldSeed);

  // Initialize sequence to burn RNG correctly
  NoiseOctaves minLimit(random, 16);
  NoiseOctaves maxLimit(random, 16);
  NoiseOctaves mainNoise(random, 8); // Target

  // Params from generateTerrain for Main Noise:
  // Scale X/Z = 684.412 / 80.0
  // Scale Y   = 684.412 / 160.0

  std::vector<double> noise(5 * 17 * 5);

  // Y=0, Z=0 (Standard 3D noise coords)
  mainNoise.generate(noise.data(), 0, 0, 0, 5, 17, 5, 684.412 / 80.0,
                     684.412 / 160.0, 684.412 / 80.0);

  std::cout << "=== C++ MAIN NOISE CHECK ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  std::cout << "idx 0: " << noise[0] << std::endl;
  std::cout << "idx 8: " << noise[8] << std::endl;

  return 0;
}