#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  int64_t worldSeed = -100L;
  JavaRandom random(worldSeed);

  // Initialize in order
  NoiseOctaves minLimitNoise(random, 16);
  NoiseOctaves maxLimitNoise(random, 16);
  NoiseOctaves mainNoise(random, 8);
  // ... others don't matter for this test

  // Generate 3D Noise for Chunk 0,0
  // Parameters from calculateDensity:
  // sizeX=5, sizeY=17, sizeZ=5
  // scale coordinates...

  int sizeX = 5;
  int sizeY = 17;
  int sizeZ = 5;
  std::vector<double> minNoiseArray(sizeX * sizeY * sizeZ);

  int chunkX = 0;
  int chunkZ = 0;
  double coordScale = 684.412;
  double heightScale = 684.412;

  // Parameters: array, x, y, z, sizeX, sizeY, sizeZ, scaleX, scaleY, scaleZ
  // FIX: Pass chunkZ*4 as Y, and 0 as Z.
  minLimitNoise.generate(minNoiseArray.data(), chunkX * 4, chunkZ * 4, 0, sizeX,
                         sizeY, sizeZ, coordScale, heightScale, coordScale);

  std::cout << "=== C++ 3D NOISE CHECK ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  // Print first 5 values
  for (int i = 0; i < 5; i++) {
    std::cout << "idx " << i << ": " << minNoiseArray[i] << std::endl;
  }

  // Print a value from the middle (y=8)
  int midIdx = (0 * 5 + 0) * 17 + 8;
  std::cout << "idx " << midIdx << ": " << minNoiseArray[midIdx] << std::endl;

  return 0;
}