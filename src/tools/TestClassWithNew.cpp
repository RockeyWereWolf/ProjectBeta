#include "../core/JavaRandom.hpp"
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

// A minimal replica of your WorldGen class, but using pointers and 'new'
class TestGeneratorWithNew {
public:
  JavaRandom random;

  // Pointers to the noise generators
  NoiseOctaves *minLimitNoise;
  NoiseOctaves *maxLimitNoise;
  NoiseOctaves *mainNoise;
  NoiseOctaves *surfaceNoise;
  NoiseOctaves *valNoise;
  NoiseOctaves *scaleNoise;
  NoiseOctaves *depthNoise;
  NoiseOctaves *forestNoise;

  std::vector<double> sandNoise;
  std::vector<double> gravelNoise;
  std::vector<double> stoneNoise;

  // Constructor allocates with 'new'
  TestGeneratorWithNew(int64_t seed) : random(seed) {
    minLimitNoise = new NoiseOctaves(random, 16);
    maxLimitNoise = new NoiseOctaves(random, 16);
    mainNoise = new NoiseOctaves(random, 8);
    surfaceNoise = new NoiseOctaves(random, 4);
    valNoise = new NoiseOctaves(random, 4);
    scaleNoise = new NoiseOctaves(random, 10);
    depthNoise = new NoiseOctaves(random, 16);
    forestNoise = new NoiseOctaves(random, 8);

    sandNoise.resize(256);
    gravelNoise.resize(256);
    stoneNoise.resize(256);
  }

  // Destructor to clean up memory
  ~TestGeneratorWithNew() {
    delete minLimitNoise;
    delete maxLimitNoise;
    delete mainNoise;
    delete surfaceNoise;
    delete valNoise;
    delete scaleNoise;
    delete depthNoise;
    delete forestNoise;
  }

  void runTest() {
    // ... (The rest of the test logic is the same as the previous test)
    int64_t chunkX = 0;
    int64_t chunkZ = 0;
    random.setSeed(chunkX * 341873128712L + chunkZ * 132897987541L);

    double scale = 0.03125;
    surfaceNoise->generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16,
                           16, 1, scale, scale, 1.0);
    surfaceNoise->generate(gravelNoise.data(), chunkX * 16, 109.0134,
                           chunkZ * 16, 16, 1, 16, scale, 1.0, scale);
    valNoise->generate(stoneNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16,
                       1, scale * 2.0, scale * 2.0, 1.0);

    std::cout << "=== C++ CLASS + NEW ALLOCATION TRACE (Chunk 0,0) ==="
              << std::endl;

    for (int x = 0; x < 4; ++x) {
      int idx = x; // z is always 0
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
  TestGeneratorWithNew generator(-100L);
  generator.runTest();
  return 0;
}