
#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  int64_t seed = 12345;
  JavaRandom listRand(seed);

  // Exact same sequence of allocations as Java
  NoiseOctaves field_912_k(listRand, 16);
  NoiseOctaves field_911_l(listRand, 16);
  NoiseOctaves field_910_m(listRand, 8);
  NoiseOctaves field_909_n(listRand, 4); // Used for Sand

  std::vector<double> sandNoise(256);
  double scale = 0.03125;
  int chunkX = 0;
  int chunkZ = 0;

  // In Java: generate(sandNoise, chunkX*16, chunkZ*16, 0.0, 16, 16, 1, scale,
  // scale, 1.0); Arguments: Array, x, y, z, sizeX, sizeY, sizeZ, scaleX,
  // scaleY, scaleZ

  // In C++ generate signature:
  // void generate(double *noiseArray, int x, int y, int z, int xSize, int
  // ySize,
  //            int zSize, double xScale, double yScale, double zScale)

  // Direct mapping
  field_909_n.generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16, 1,
                       scale, scale, 1.0);

  std::cout << "=== C++ SAND NOISE (Chunk 0,0) ===" << std::endl;
  std::cout << std::fixed << std::setprecision(10);
  for (int i = 0; i < 16; i++) {
    std::cout << "Index " << i << ": " << sandNoise[i] << std::endl;
  }
  std::cout << "..." << std::endl;
  for (int i = 0; i < 4; i++) {
    int idx = 16 + i;
    std::cout << "Index " << idx << ": " << sandNoise[idx] << std::endl;
  }

  return 0;
}
