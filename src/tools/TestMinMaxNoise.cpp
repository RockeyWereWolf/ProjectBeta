#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  int64_t seed = -100L;
  JavaRandom random(seed);

  NoiseOctaves minLimit(random, 16);
  NoiseOctaves maxLimit(random, 16);
  // Burn the rest to match sequence
  NoiseOctaves main(random, 8);

  int sizeX = 5, sizeY = 17, sizeZ = 5;
  double coordScale = 684.412;
  double heightScale = 684.412;

  std::vector<double> minN(sizeX * sizeY * sizeZ);
  std::vector<double> maxN(sizeX * sizeY * sizeZ);

  // Y=0, Z=0 (Standard 3D coords)
  minLimit.generate(minN.data(), 0, 0, 0, sizeX, sizeY, sizeZ, coordScale,
                    heightScale, coordScale);

  maxLimit.generate(maxN.data(), 0, 0, 0, sizeX, sizeY, sizeZ, coordScale,
                    heightScale, coordScale);

  std::cout << "=== C++ MIN/MAX NOISE CHECK ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  std::cout << "Min[0]: " << minN[0] << std::endl;
  std::cout << "Max[0]: " << maxN[0] << std::endl;
  std::cout << "Min[8]: " << minN[8] << std::endl;
  std::cout << "Max[8]: " << maxN[8] << std::endl;

  return 0;
}