
#include "../world/BiomeManager.hpp"
#include <iostream>

int main() {
  int64_t seed = -100; // Hardcoded seed
  BiomeManager bio(seed);

  std::cout << "BiomeID Check for Seed " << seed << ":" << std::endl;
  for (int x = 0; x <= 16; x += 16) {
    for (int z = 0; z <= 16; z += 16) {
      double temp, rain;
      int rx = x;
      int rz = z;
      // Sample offset is usually globex+8, globez+8 for features
      // But for raw biome map, we just check output.
      bio.getTempRain(rx, rz, temp, rain);
      BiomeID id = bio.getBiomeFromValues(temp, rain);
      std::cout << "At (" << rx << "," << rz << "): Temp=" << temp
                << " Rain=" << rain << " ID=" << (int)id << std::endl;
    }
  }
  return 0;
}
