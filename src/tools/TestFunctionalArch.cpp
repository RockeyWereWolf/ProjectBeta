#include "../core/JavaRandom.hpp"
#include "../world/BiomeManager.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

class TestGenerator {
private:
  int64_t worldSeed;

  // Pointers, as in the proposed fix
  NoiseOctaves *minLimitNoise;
  NoiseOctaves *maxLimitNoise;
  NoiseOctaves *mainNoise;
  NoiseOctaves *surfaceNoise;
  NoiseOctaves *valNoise;
  NoiseOctaves *scaleNoise;
  NoiseOctaves *depthNoise;
  NoiseOctaves *forestNoise;

  BiomeManager biomeManager;

  std::vector<double> sandNoise;
  std::vector<double> gravelNoise;
  std::vector<double> stoneNoise;

public:
  TestGenerator(int64_t wSeed) : worldSeed(wSeed), biomeManager(wSeed) {

    sandNoise.resize(256);
    gravelNoise.resize(256);
    stoneNoise.resize(256);

    // Call the initializer
    init();
  }

  ~TestGenerator() {
    delete minLimitNoise;
    delete maxLimitNoise;
    delete mainNoise;
    delete surfaceNoise;
    delete valNoise;
    delete scaleNoise;
    delete depthNoise;
    delete forestNoise;
  }

  // This is the key: Random object is LOCAL to this function
  void init() {
    JavaRandom random(worldSeed);

    minLimitNoise = new NoiseOctaves(random, 16);
    maxLimitNoise = new NoiseOctaves(random, 16);
    mainNoise = new NoiseOctaves(random, 8);
    surfaceNoise = new NoiseOctaves(random, 4);
    valNoise = new NoiseOctaves(random, 4);
    scaleNoise = new NoiseOctaves(random, 10);
    depthNoise = new NoiseOctaves(random, 16);
    forestNoise = new NoiseOctaves(random, 8);
  }

  void runTest() {
    int64_t chunkX = 0;
    int64_t chunkZ = 0;

    // This random is for the replaceBlocksForBiome logic.
    // We reuse the existing random object logic from TestDecisions
    // but since we are in a method, we create a local one.
    JavaRandom random(0);
    random.setSeed(chunkX * 341873128712L + chunkZ * 132897987541L);

    double scale = 0.03125;

    surfaceNoise->generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16,
                           16, 1, scale, scale, 1.0);
    surfaceNoise->generate(gravelNoise.data(), chunkX * 16, 109.0134,
                           chunkZ * 16, 16, 1, 16, scale, 1.0, scale);
    valNoise->generate(stoneNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16,
                       1, scale * 2.0, scale * 2.0, 1.0);

    std::cout << "=== C++ FUNCTIONAL ARCHITECTURE TRACE (Chunk 0,0) ==="
              << std::endl;
    std::cout << std::fixed << std::setprecision(17);

    for (int x = 0; x < 4; ++x) {
      // Only checking z=0 row
      int z = 0;
      int idx = x + z * 16;

      double sandRnd = random.nextDouble();
      bool genSand = sandNoise[idx] + sandRnd * 0.2 > 0.0;

      double gravelRnd = random.nextDouble();
      bool genGravel = gravelNoise[idx] + gravelRnd * 0.2 > 3.0;

      double stoneRnd = random.nextDouble();
      int stoneDepth = (int)(stoneNoise[idx] / 3.0 + 3.0 + stoneRnd * 0.25);

      std::cout << "Col (" << x << ",0): ... Depth: " << stoneDepth
                << std::endl;
    }
  }
};

int main() {
  TestGenerator generator(-100L);
  generator.runTest();
  return 0;
}