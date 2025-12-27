#include "../core/JavaRandom.hpp"
#include "../world/BiomeManager.hpp" // For BiomeID
#include "../world/Noise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

// Mock Chunk to track block placement (only what we need)
struct MockChunk {
  int blocks[16][128][16]; // x, y, z
  MockChunk() {
    for (int x = 0; x < 16; x++)
      for (int y = 0; y < 128; y++)
        for (int z = 0; z < 16; z++)
          blocks[x][y][z] = 0; // Air
  }
  void setBlock(int x, int y, int z, int id) { blocks[x][y][z] = id; }
  int getBlock(int x, int y, int z) { return blocks[x][y][z]; }
};

int main() {
  int64_t chunkX = 0;
  int64_t chunkZ = 0;
  int64_t worldSeed = -100L;

  // 1. Setup RNG and Noise exactly as WorldGen does
  // Order matters for RNG state burn!
  JavaRandom random(worldSeed);
  NoiseOctaves minLimitNoise(random, 16);
  NoiseOctaves maxLimitNoise(random, 16);
  NoiseOctaves mainNoise(random, 8);
  NoiseOctaves surfaceNoise(random, 4);
  NoiseOctaves valNoise(random, 4);
  NoiseOctaves scaleNoise(random, 10);
  NoiseOctaves depthNoise(random, 16);
  NoiseOctaves forestNoise(random, 8);

  // 2. Generate Noise Arrays (for Chunk 0,0)
  std::vector<double> sandNoise(256);
  std::vector<double> gravelNoise(256);
  std::vector<double> stoneNoise(256);

  // Reset Seed for Biome Replacement
  random.setSeed(chunkX * 341873128712L + chunkZ * 132897987541L);

  double scale = 0.03125;
  surfaceNoise.generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16,
                        1, scale, scale, 1.0);
  surfaceNoise.generate(gravelNoise.data(), chunkX * 16, 109.0134, chunkZ * 16,
                        16, 1, 16, scale, 1.0, scale);
  valNoise.generate(stoneNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16, 1,
                    scale * 2.0, scale * 2.0, 1.0);

  std::cout << "=== C++ STANDALONE DECISION TRACE (Chunk 0,0) ===" << std::endl;
  std::cout << std::fixed << std::setprecision(17);

  MockChunk chunk;
  // Simulate filling chunk with stone/water (simplified terrain gen)
  // We just fill everything below 64 with stone for the test,
  // or we can just assume getBlock returns Stone (1) if we want to test
  // replacement logic.
  for (int x = 0; x < 16; x++)
    for (int z = 0; z < 16; z++)
      for (int y = 0; y < 90; y++)
        chunk.setBlock(x, y, z, 1);

  // 3. Run the Logic Loop (First 4 columns only)
  for (int x = 0; x < 4; ++x) {
    for (int z = 0; z < 1; ++z) { // Only z=0 line
      int idx = x + z * 16;

      double sandRnd = random.nextDouble();
      bool genSand = sandNoise[idx] + sandRnd * 0.2 > 0.0;

      double gravelRnd = random.nextDouble();
      bool genGravel = gravelNoise[idx] + gravelRnd * 0.2 > 3.0;

      double stoneRnd = random.nextDouble();
      int stoneDepth = (int)(stoneNoise[idx] / 3.0 + 3.0 + stoneRnd * 0.25);

      std::cout << "Col (" << x << "," << z << "):" << std::endl;
      std::cout << "  Noise[S]: " << sandNoise[idx] << " Rnd: " << sandRnd
                << " -> Sand: " << genSand << std::endl;
      std::cout << "  Noise[G]: " << gravelNoise[idx] << " Rnd: " << gravelRnd
                << " -> Gravel: " << genGravel << std::endl;
      std::cout << "  Noise[D]: " << stoneNoise[idx] << " Rnd: " << stoneRnd
                << " -> Depth: " << stoneDepth << std::endl;

      // Simulate the Vertical Loop to catch Bedrock/Sandstone RNG calls
      int remainingDepth = -1;
      int topBlock = 2;    // Grass
      int fillerBlock = 3; // Dirt

      // Hardcode Forest Biome behavior for 0,0
      // BiomeGenBase b = ... (Forest)
      // topBlock = b.topBlock (2)
      // fillerBlock = b.fillerBlock (3)

      for (int y = 127; y >= 0; --y) {
        // BEDROCK CHECK
        int bedrockRnd = random.nextInt(5);
        if (y <= 5) { // Only print near bottom to reduce spam
          std::cout << "  y=" << y << " BedrockRnd: " << bedrockRnd
                    << std::endl;
        }

        if (y <= bedrockRnd) {
          chunk.setBlock(x, y, z, 7);
          continue;
        }

        int blk = chunk.getBlock(x, y, z);

        if (blk == 0) {
          remainingDepth = -1;
        } else if (blk == 1) { // Stone
          if (remainingDepth == -1) {
            if (stoneDepth <= 0) {
              topBlock = 0;
              fillerBlock = 1;
            } else if (y >= 60 && y <= 65) {
              topBlock = 2;
              fillerBlock = 3; // Forest defaults
              if (genGravel) {
                topBlock = 0;
                fillerBlock = 13;
              }
              if (genSand) {
                topBlock = 12;
                fillerBlock = 12;
              }
            }
            if (y < 64 && topBlock == 0)
              topBlock = 9;

            remainingDepth = stoneDepth;
            // Set blocks...
          } else if (remainingDepth > 0) {
            --remainingDepth;
            // SANDSTONE CHECK
            if (remainingDepth == 0 && fillerBlock == 12) {
              int sandstoneRnd = random.nextInt(4);
              std::cout << "  y=" << y << " SandstoneRnd: " << sandstoneRnd
                        << " (Sandstone Gen)" << std::endl;
              remainingDepth = sandstoneRnd;
              fillerBlock = 24;
            }
          }
        }
      }
    }
  }
  return 0;
}