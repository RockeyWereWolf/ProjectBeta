#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  int64_t worldSeed = -100L;
  JavaRandom random(worldSeed);

  // Initialize generators in the exact order of WorldGen to get the correct RNG
  // state
  NoiseOctaves minLimitNoise(random, 16);
  NoiseOctaves maxLimitNoise(random, 16);
  NoiseOctaves mainNoise(random, 8);
  NoiseOctaves surfaceNoise(random, 4);
  NoiseOctaves valNoise(random, 4); // This is the one we are testing

  std::vector<double> stoneNoise(256);

  // Generate noise for chunk (0,0) with parameters from replaceBlocksForBiome
  int chunkX = 0;
  int chunkZ = 0;
  double scale = 0.03125;
  valNoise.generate(stoneNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16, 1,
                    scale * 2.0, scale * 2.0, 1.0);

  std::cout << "=== C++ STONE NOISE CHECK ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  for (int i = 0; i < 4; i++) {
    std::cout << "idx " << i << ": " << stoneNoise[i] << std::endl;
  }

  return 0;
}