#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp" // Changed from MineNoise.hpp based on your file tree
#include <iomanip>
#include <iostream>
#include <vector>

// We need to define NoiseOctaves locally or include WorldGen's definition.
// Since NoiseOctaves is likely in Noise.hpp or WorldGen.hpp, we'll try to use
// the one from Noise.hpp. If your NoiseOctaves class is inside WorldGen.hpp, we
// might need to copy it or include WorldGen.hpp. Based on your previous file
// tree, you have src/world/Noise.hpp. I will assume NoiseOctaves is defined
// there or in a related header.

int main() {
  int64_t worldSeed = -100L;

  // 1. Initialize RNG with World Seed
  JavaRandom random(worldSeed);

  // 2. Initialize Generators in the EXACT order of WorldGen constructor
  // WorldGen(int64_t wSeed) : ... minLimitNoise(random, 16),
  // maxLimitNoise(random, 16),
  //                           mainNoise(random, 8), surfaceNoise(random, 4) ...

  // We must instantiate these to burn the RNG state correctly so surfaceNoise
  // gets the right seed.
  NoiseOctaves minLimitNoise(random, 16);
  NoiseOctaves maxLimitNoise(random, 16);
  NoiseOctaves mainNoise(random, 8);

  // This is the one we care about
  NoiseOctaves surfaceNoise(random, 4);

  // 3. Prepare output array
  // sandNoise size is 256
  std::vector<double> sandNoise(256);

  // 4. Generate Noise for Chunk 0,0
  // Logic from replaceBlocksForBiome:
  // surfaceNoise.generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16,
  // 16, 1, scale, scale, 1.0);
  int chunkX = 0;
  int chunkZ = 0;
  double scale = 0.03125;

  surfaceNoise.generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16,
                        1, scale, scale, 1.0);

  // 5. Print Results
  std::cout << "=== C++ NOISE CHECK (Chunk 0,0) ===" << std::endl;
  std::cout << "Seed: " << worldSeed << std::endl;
  std::cout << "First 5 values of sandNoise:" << std::endl;
  std::cout << std::fixed << std::setprecision(20);

  for (int i = 0; i < 5; i++) {
    std::cout << "idx " << i << ": " << sandNoise[i] << std::endl;
  }

  return 0;
}