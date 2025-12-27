#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

// Helper to peek at private members
struct PerlinPublic {
  int permutations[512];
  double xCoord, yCoord, zCoord;
};

int main() {
  int64_t seed = -100L;
  JavaRandom random(seed);

  // Initialize generators in the exact order of WorldGen
  NoiseOctaves minLimit(random, 16);
  NoiseOctaves maxLimit(random, 16);
  NoiseOctaves main(random, 8);
  NoiseOctaves surface(random, 4);
  NoiseOctaves val(random, 4);

  // The next one is scaleNoise (10 octaves).
  // We manually create the first octave to inspect it.
  PerlinNoise scaleOct0(random);

  PerlinPublic *p = (PerlinPublic *)&scaleOct0;

  std::cout << "=== C++ SCALE NOISE INIT CHECK ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  std::cout << "xCoord: " << p->xCoord << std::endl;

  return 0;
}