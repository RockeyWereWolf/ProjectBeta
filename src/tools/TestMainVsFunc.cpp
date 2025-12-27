#include "../core/JavaRandom.hpp"
#include "../world/BiomeManager.hpp" // Included for completeness
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

// Mock Chunk struct from TestDecisions
struct MockChunk {
  int blocks[16][128][16];
  MockChunk() {
    for (int x = 0; x < 16; x++)
      for (int y = 0; y < 128; y++)
        for (int z = 0; z < 16; z++)
          blocks[x][y][z] = 0;
  }
  void setBlock(int x, int y, int z, int id) { blocks[x][y][z] = id; }
  int getBlock(int x, int y, int z) { return blocks[x][y][z]; }
};

// This function now contains the IDENTICAL code from the working
// TestDecisions.cpp's main()
void run_the_test() {
  int64_t chunkX = 0;
  int64_t chunkZ = 0;
  int64_t worldSeed = -100L;

  JavaRandom random(worldSeed);
  NoiseOctaves minLimitNoise(random, 16);
  NoiseOctaves maxLimitNoise(random, 16);
  NoiseOctaves mainNoise(random, 8);
  NoiseOctaves surfaceNoise(random, 4);
  NoiseOctaves valNoise(random, 4);
  NoiseOctaves scaleNoise(random, 10);
  NoiseOctaves depthNoise(random, 16);
  NoiseOctaves forestNoise(random, 8);

  std::vector<double> sandNoise(256);
  std::vector<double> gravelNoise(256);
  std::vector<double> stoneNoise(256);

  random.setSeed(chunkX * 341873128712L + chunkZ * 132897987541L);

  double scale = 0.03125;
  surfaceNoise.generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16,
                        1, scale, scale, 1.0);
  surfaceNoise.generate(gravelNoise.data(), chunkX * 16, 109.0134, chunkZ * 16,
                        16, 1, 16, scale, 1.0, scale);
  valNoise.generate(stoneNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16, 1,
                    scale * 2.0, scale * 2.0, 1.0);

  std::cout << "=== FINAL TEST: main() vs function() (CORRECTED) ==="
            << std::endl;

  MockChunk chunk;
  for (int x = 0; x < 16; x++)
    for (int z = 0; z < 16; z++)
      for (int y = 0; y < 90; y++)
        chunk.setBlock(x, y, z, 1);

  for (int x = 0; x < 4; ++x) {
    for (int z = 0; z < 1; ++z) {
      int idx = x + z * 16;
      double sandRnd = random.nextDouble();
      bool genSand = sandNoise[idx] + sandRnd * 0.2 > 0.0;
      double gravelRnd = random.nextDouble();
      bool genGravel = gravelNoise[idx] + gravelRnd * 0.2 > 3.0;
      double stoneRnd = random.nextDouble();
      int stoneDepth = (int)(stoneNoise[idx] / 3.0 + 3.0 + stoneRnd * 0.25);

      std::cout << "Col (" << x << ",0): ... Depth: " << stoneDepth
                << std::endl;

      // --- CRITICAL BEDROCK LOOP (RESTORED) ---
      int remainingDepth = -1;
      int topBlock = 2;
      int fillerBlock = 3;
      for (int y = 127; y >= 0; --y) {
        int bedrockRnd = random.nextInt(5);
        if (y <= bedrockRnd)
          continue;
        int blk = chunk.getBlock(x, y, z);
        if (blk == 0) {
          remainingDepth = -1;
        } else if (blk == 1) {
          if (remainingDepth == -1) {
            remainingDepth = stoneDepth;
          } else if (remainingDepth > 0) {
            --remainingDepth;
            if (remainingDepth == 0 && fillerBlock == 12) {
              remainingDepth = random.nextInt(4);
              fillerBlock = 24;
            }
          }
        }
      }
      // --- END OF LOOP ---
    }
  }
}

int main() {
  run_the_test();
  return 0;
}