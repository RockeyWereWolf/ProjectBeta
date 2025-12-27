#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>

// Cheat to access private members
struct PerlinNoisePublic {
  int permutations[512];
  double xCoord, yCoord, zCoord;
};

int main() {
  int64_t worldSeed = -100L;
  JavaRandom random(worldSeed);

  // Octave 0 (Consume)
  PerlinNoise oct0(random);

  // Octave 1 (Target)
  PerlinNoise oct1(random);

  // Cast to public struct to inspect
  PerlinNoisePublic *p1 = (PerlinNoisePublic *)&oct1;

  std::cout << "=== C++ OCTAVE 1 CHECK ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  std::cout << "x: " << p1->xCoord << std::endl;
  std::cout << "y: " << p1->yCoord << std::endl;
  std::cout << "z: " << p1->zCoord << std::endl;
  std::cout << "Perm[0]: " << p1->permutations[0] << std::endl;

  return 0;
}