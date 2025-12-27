#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

class TestGenerator {
private:
  int64_t worldSeed;
  // Pointers to noise generators
  NoiseOctaves *minLimitNoise;
  NoiseOctaves *maxLimitNoise;
  NoiseOctaves *mainNoise;
  NoiseOctaves *surfaceNoise;
  NoiseOctaves *valNoise;
  NoiseOctaves *scaleNoise;
  NoiseOctaves *depthNoise;
  NoiseOctaves *forestNoise;

public:
  // This random is ONLY for the biome replacement logic
  JavaRandom logic_random;

  // Noise arrays
  std::vector<double> sandNoise;
  std::vector<double> gravelNoise;
  std::vector<double> stoneNoise;

  TestGenerator(int64_t wSeed) : worldSeed(wSeed), logic_random(0) {
    init(); // Constructor calls the initializer
    sandNoise.resize(256);
    gravelNoise.resize(256);
    stoneNoise.resize(256);
  }

  // Destructor to clean up memory
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

  void init() {
    // Create a LOCAL, temporary random object on the stack just for
    // initialization
    JavaRandom init_random(worldSeed);

    // Allocate members one-by-one using the local RNG
    minLimitNoise = new NoiseOctaves(init_random, 16);
    maxLimitNoise = new NoiseOctaves(init_random, 16);
    mainNoise = new NoiseOctaves(init_random, 8);
    surfaceNoise = new NoiseOctaves(init_random, 4);
    valNoise = new NoiseOctaves(init_random, 4);
    scaleNoise = new NoiseOctaves(init_random, 10);
    depthNoise = new NoiseOctaves(init_random, 16);
    forestNoise = new NoiseOctaves(init_random, 8);
  }

  void runTest() {
    int64_t chunkX = 0;
    int64_t chunkZ = 0;

    logic_random.setSeed(chunkX * 341873128712L + chunkZ * 132897987541L);

    double scale = 0.03125;
    surfaceNoise->generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16,
                           16, 1, scale, scale, 1.0);
    surfaceNoise->generate(gravelNoise.data(), chunkX * 16, 109.0134,
                           chunkZ * 16, 16, 1, 16, scale, 1.0,
                           scale); // Note: Corrected surfaceNoise call
    valNoise->generate(stoneNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16,
                       1, scale * 2.0, scale * 2.0, 1.0);

    std::cout << "=== C++ ISOLATED INITIALIZER TRACE (Chunk 0,0) ==="
              << std::endl;

    for (int x = 0; x < 4; ++x) {
      int idx = x;
      double sandRnd = logic_random.nextDouble();
      double gravelRnd = logic_random.nextDouble();
      double stoneRnd = logic_random.nextDouble();
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