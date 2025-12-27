#include "../core/JavaRandom.hpp"
#include "../world/BiomeManager.hpp" // Not strictly needed, but good for structure
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

// A minimal replica of your WorldGen class structure
class TestGenerator {
public:
  JavaRandom random;
  NoiseOctaves minLimitNoise;
  NoiseOctaves maxLimitNoise;
  NoiseOctaves mainNoise;
  NoiseOctaves surfaceNoise;
  NoiseOctaves valNoise;
  NoiseOctaves scaleNoise;
  NoiseOctaves depthNoise;
  NoiseOctaves forestNoise;

  std::vector<double> sandNoise;
  std::vector<double> gravelNoise;
  std::vector<double> stoneNoise;

  // Constructor with initializer list, exactly like WorldGen
  TestGenerator(int64_t seed)
      : random(seed), minLimitNoise(random, 16), maxLimitNoise(random, 16),
        mainNoise(random, 8), surfaceNoise(random, 4), valNoise(random, 4),
        scaleNoise(random, 10), depthNoise(random, 16), forestNoise(random, 8) {
    sandNoise.resize(256);
    gravelNoise.resize(256);
    stoneNoise.resize(256);
  }

  void runTest() {
    int64_t chunkX = 0;
    int64_t chunkZ = 0;

    // Reset Seed for Biome Replacement (using the member 'random' object)
    random.setSeed(chunkX * 341873128712L + chunkZ * 132897987541L);

    double scale = 0.03125;
    surfaceNoise.generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16,
                          1, scale, scale, 1.0);
    surfaceNoise.generate(gravelNoise.data(), chunkX * 16, 109.0134,
                          chunkZ * 16, 16, 1, 16, scale, 1.0, scale);
    valNoise.generate(stoneNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16, 1,
                      scale * 2.0, scale * 2.0, 1.0);

    std::cout << "=== C++ CLASS STRUCTURE DECISION TRACE (Chunk 0,0) ==="
              << std::endl;
    std::cout << std::fixed << std::setprecision(17);

    for (int x = 0; x < 4; ++x) {
      for (int z = 0; z < 1; ++z) {
        int idx = x + z * 16;
        double sandRnd = random.nextDouble();
        bool genSand = sandNoise[idx] + sandRnd * 0.2 > 0.0;
        double gravelRnd = random.nextDouble();
        bool genGravel = gravelNoise[idx] + gravelRnd * 0.2 > 3.0;
        double stoneRnd = random.nextDouble();
        int stoneDepth = (int)(stoneNoise[idx] / 3.0 + 3.0 + stoneRnd * 0.25);

        std::cout << "Col (" << x << "," << z << "): ... Depth: " << stoneDepth
                  << std::endl;
      }
    }
  }
};

int main() {
  TestGenerator generator(-100L);
  generator.runTest();
  return 0;
}