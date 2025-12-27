#pragma once

#include "../core/JavaRandom.hpp"
#include "BiomeManager.hpp"
#include "Chunk.hpp"
#include "MathHelper.hpp"
#include "Noise.hpp"
#include "WorldGenMinable.hpp"
#include "WorldGenTrees.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

class WorldGen {
private:
  int64_t worldSeed;
  JavaRandom random;

  NoiseOctaves minLimitNoise;
  NoiseOctaves maxLimitNoise;
  NoiseOctaves mainNoise;
  NoiseOctaves surfaceNoise;
  NoiseOctaves valNoise;
  NoiseOctaves scaleNoise;
  NoiseOctaves depthNoise;
  NoiseOctaves forestNoise;

  BiomeManager biomeManager;

  std::vector<double> sandNoise;
  std::vector<double> gravelNoise;
  std::vector<double> stoneNoise;

public:
  WorldGen(int64_t wSeed)
      : worldSeed(wSeed), random(wSeed), minLimitNoise(random, 16),
        maxLimitNoise(random, 16), mainNoise(random, 8),
        surfaceNoise(random, 4), valNoise(random, 4), scaleNoise(random, 10),
        depthNoise(random, 16), forestNoise(random, 8), biomeManager(wSeed) {
    sandNoise.resize(256);
    gravelNoise.resize(256);
    stoneNoise.resize(256);
  }

  std::vector<double> calculateDensity(int chunkX, int chunkZ) {
    int sizeX = 5;
    int sizeY = 17;
    int sizeZ = 5;

    std::vector<double> density(sizeX * sizeY * sizeZ);
    std::vector<double> minNoiseArray(sizeX * sizeY * sizeZ);
    std::vector<double> maxNoiseArray(sizeX * sizeY * sizeZ);
    std::vector<double> mainNoiseArray(sizeX * sizeY * sizeZ);
    std::vector<double> scaleNoiseArray(sizeX * sizeZ);
    std::vector<double> depthNoiseArray(sizeX * sizeZ);

    std::vector<double> fullTemp(16 * 16);
    std::vector<double> fullRain(16 * 16);
    biomeManager.getBiomeDataForRegion(fullTemp.data(), fullRain.data(),
                                       chunkX * 16, chunkZ * 16, 16, 16);
    std::vector<double> biomeTempArray(sizeX * sizeZ);
    std::vector<double> biomeRainArray(sizeX * sizeZ);

    int step = 16 / sizeX;
    for (int x = 0; x < sizeX; ++x) {
      int xIdx = x * step + step / 2;
      for (int z = 0; z < sizeZ; ++z) {
        int zIdx = z * step + step / 2;
        int destIdx = x * sizeZ + z;
        int srcIdx = xIdx * 16 + zIdx;
        biomeTempArray[destIdx] = fullTemp[srcIdx];
        biomeRainArray[destIdx] = fullRain[srcIdx];
      }
    }

    double coordScale = 684.412;
    double heightScale = 684.412;

    scaleNoise.generate(scaleNoiseArray.data(), chunkX * 4, 10, chunkZ * 4,
                        sizeX, 1, sizeZ, 1.121, 1.0, 1.121);
    depthNoise.generate(depthNoiseArray.data(), chunkX * 4, 10, chunkZ * 4,
                        sizeX, 1, sizeZ, 200.0, 1.0, 200.0);
    minLimitNoise.generate(minNoiseArray.data(), chunkX * 4, 0, chunkZ * 4,
                           sizeX, sizeY, sizeZ, coordScale, heightScale,
                           coordScale);
    maxLimitNoise.generate(maxNoiseArray.data(), chunkX * 4, 0, chunkZ * 4,
                           sizeX, sizeY, sizeZ, coordScale, heightScale,
                           coordScale);
    mainNoise.generate(mainNoiseArray.data(), chunkX * 4, 0, chunkZ * 4, sizeX,
                       sizeY, sizeZ, coordScale / 80.0, heightScale / 160.0,
                       coordScale / 80.0);

    int main3DIdx = 0;
    int biome2DIdx = 0;

    for (int x = 0; x < sizeX; ++x) {
      for (int z = 0; z < sizeZ; ++z) {

        // --- DEBUG START ---
        if (chunkX == 0 && chunkZ == 0 && x == 0 && z == 0) {
          std::cout << "=== C++ DENSITY INPUTS (0,0) ===" << std::endl;
          std::cout << std::fixed << std::setprecision(17);
          std::cout << "Biome Temp:  " << biomeTempArray[biome2DIdx]
                    << std::endl;
          std::cout << "Biome Rain:  " << biomeRainArray[biome2DIdx]
                    << std::endl;
          std::cout << "Scale Noise: " << scaleNoiseArray[biome2DIdx]
                    << std::endl;
          std::cout << "Depth Noise: " << depthNoiseArray[biome2DIdx]
                    << std::endl;
        }
        // --- DEBUG END ---

        double currentBiomeTemp = biomeTempArray[biome2DIdx];
        double currentBiomeRain = biomeRainArray[biome2DIdx];

        double biomeInfluence = currentBiomeRain * currentBiomeTemp;
        biomeInfluence = 1.0 - biomeInfluence;
        biomeInfluence *= biomeInfluence;
        biomeInfluence *= biomeInfluence;
        biomeInfluence = 1.0 - biomeInfluence;

        double scaleVal = (scaleNoiseArray[biome2DIdx] + 256.0) / 512.0;
        scaleVal *= biomeInfluence;
        if (scaleVal > 1.0)
          scaleVal = 1.0;

        double depthVal = depthNoiseArray[biome2DIdx] / 8000.0;
        if (depthVal < 0.0)
          depthVal = -depthVal * 0.3;
        depthVal = depthVal * 3.0 - 2.0;
        if (depthVal < 0.0) {
          depthVal /= 2.0;
          if (depthVal < -1.0)
            depthVal = -1.0;
          depthVal /= 1.4;
          depthVal /= 2.0;
          scaleVal = 0.0;
        } else {
          if (depthVal > 1.0)
            depthVal = 1.0;
          depthVal /= 8.0;
        }
        if (scaleVal < 0.0)
          scaleVal = 0.0;
        scaleVal += 0.5;

        depthVal = depthVal * (double)sizeY / 16.0;

        double volatility = scaleVal;
        double heightOffset = (double)sizeY / 2.0 + depthVal * 4.0;

        for (int y = 0; y < sizeY; ++y) {
          int idx = (x * sizeZ + z) * sizeY + y;

          double heightBias = ((double)y - heightOffset) * 12.0 / volatility;

          if (heightBias < 0.0)
            heightBias *= 4.0;

          double min = minNoiseArray[idx] / 512.0;
          double max = maxNoiseArray[idx] / 512.0;
          double main = (mainNoiseArray[idx] / 10.0 + 1.0) / 2.0;

          double densityVal =
              (main < 0.0) ? min
                           : ((main > 1.0) ? max : min + (max - min) * main);
          densityVal -= heightBias;

          if (y > sizeY - 4) {
            double fade = (double)((float)(y - (sizeY - 4)) / 3.0f);
            densityVal = densityVal * (1.0 - fade) + -10.0 * fade;
          }
          density[idx] = densityVal;
        }
        biome2DIdx++;
      }
    }

    // --- ADD DEBUG PRINT HERE ---
    if (chunkX == 0 && chunkZ == 0) {
      std::cout << "=== C++ DENSITY OUTPUT CHECK ===" << std::endl;
      std::cout << std::fixed << std::setprecision(20);

      std::cout << "Density[0] (y=0):  " << density[0] << std::endl;
      std::cout << "Density[7] (y=7):  " << density[7] << std::endl;
      std::cout << "Density[8] (y=8):  " << density[8] << std::endl;
      std::cout << "Density[9] (y=9):  " << density[9] << std::endl;
      std::cout << "Density[16] (y=16): " << density[16] << std::endl;
    }
    // ----------------------------

    return density;
  }

  void replaceBlocksForBiome(class Chunk &chunk) {
    int chunkX = chunk.chunkX;
    int chunkZ = chunk.chunkZ;

    random.setSeed((int64_t)chunkX * 341873128712L +
                   (int64_t)chunkZ * 132897987541L);

    bool isDebugChunk = (chunkX == 0 && chunkZ == 0);

    double scale = 0.03125;

    surfaceNoise.generate(sandNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16,
                          1, scale, scale, 1.0);

    surfaceNoise.generate(gravelNoise.data(), chunkX * 16, 109.0134,
                          chunkZ * 16, 16, 1, 16, scale, 1.0, scale);

    valNoise.generate(stoneNoise.data(), chunkX * 16, chunkZ * 16, 0, 16, 16, 1,
                      scale * 2.0, scale * 2.0, 1.0);

    if (isDebugChunk) {
      std::cout << "=== C++ RNG PARITY CHECK (Chunk 0,0) ===" << std::endl;
      std::cout << "--- Column (0,0) ---" << std::endl;
      std::cout << std::fixed << std::setprecision(17);
      double sandRNG = random.nextDouble();
      std::cout << "1. Sand Check (nextDouble):      " << sandRNG << std::endl;
      double gravelRNG = random.nextDouble();
      std::cout << "2. Gravel Check (nextDouble):    " << gravelRNG
                << std::endl;
      double stoneRNG = random.nextDouble();
      std::cout << "3. Stone Depth (nextDouble):     " << stoneRNG << std::endl;
      int bedrockRNG_y5 = random.nextInt(5);
      std::cout << "4. Bedrock y=5 (nextInt(5)):     " << bedrockRNG_y5
                << std::endl;
      int bedrockRNG_y4 = random.nextInt(5);
      std::cout << "5. Bedrock y=4 (nextInt(5)):     " << bedrockRNG_y4
                << std::endl;
      int bedrockRNG_y3 = random.nextInt(5);
      std::cout << "6. Bedrock y=3 (nextInt(5)):     " << bedrockRNG_y3
                << std::endl;
      int bedrockRNG_y2 = random.nextInt(5);
      std::cout << "7. Bedrock y=2 (nextInt(5)):     " << bedrockRNG_y2
                << std::endl;
      int bedrockRNG_y1 = random.nextInt(5);
      std::cout << "8. Bedrock y=1 (nextInt(5)):     " << bedrockRNG_y1
                << std::endl;

      random.setSeed((int64_t)chunkX * 341873128712L +
                     (int64_t)chunkZ * 132897987541L);
    }

    for (int x = 0; x < 16; ++x) {
      for (int z = 0; z < 16; ++z) {

        int idx = x + z * 16;

        double sandRnd = random.nextDouble();
        bool genSand = sandNoise[idx] + sandRnd * 0.2 > 0.0;

        double gravelRnd = random.nextDouble();
        bool genGravel = gravelNoise[idx] + gravelRnd * 0.2 > 3.0;

        double stoneRnd = random.nextDouble();
        int stoneDepth = (int)(stoneNoise[idx] / 3.0 + 3.0 + stoneRnd * 0.25);

        // DEBUG PRINT
        if (chunkX == 0 && chunkZ == 0 && z == 0 && x < 4) {
          std::cout << "Col (" << x << "," << z << "):" << std::endl;
          std::cout << std::fixed << std::setprecision(17);
          std::cout << "  Noise[S]: " << sandNoise[idx] << " Rnd: " << sandRnd
                    << " -> Sand: " << genSand << std::endl;
          std::cout << "  Noise[G]: " << gravelNoise[idx]
                    << " Rnd: " << gravelRnd << " -> Gravel: " << genGravel
                    << std::endl;
          std::cout << "  Noise[D]: " << stoneNoise[idx] << " Rnd: " << stoneRnd
                    << " -> Depth: " << stoneDepth << std::endl;
        }

        int remainingDepth = -1;

        BiomeProperties biome =
            biomeManager.getBiomeProps(chunkX * 16 + x, chunkZ * 16 + z);
        int topBlock = biome.topBlock;
        int fillerBlock = biome.fillerBlock;

        for (int y = 127; y >= 0; --y) {
          int bedrockRand = random.nextInt(5);

          if (y <= bedrockRand) {
            chunk.setBlock(x, y, z, 7);
            continue;
          }

          int blk = chunk.getBlock(x, y, z);

          if (blk == 0) {
            remainingDepth = -1;
          } else if (blk == 1) {
            if (remainingDepth == -1) {
              if (stoneDepth <= 0) {
                topBlock = 0;
                fillerBlock = 1;
              } else if (y >= 60 && y <= 65) {
                topBlock = biome.topBlock;
                fillerBlock = biome.fillerBlock;
                if (genGravel) {
                  topBlock = 0;
                  fillerBlock = 13;
                }
                if (genSand) {
                  topBlock = 12;
                  fillerBlock = 12;
                }
              }

              if (y < 64 && topBlock == 0) {
                topBlock = 9;
              }

              remainingDepth = stoneDepth;

              if (y >= 63) {
                chunk.setBlock(x, y, z, topBlock);
              } else {
                chunk.setBlock(x, y, z, fillerBlock);
              }
            } else if (remainingDepth > 0) {
              --remainingDepth;
              chunk.setBlock(x, y, z, fillerBlock);

              if (remainingDepth == 0 && fillerBlock == 12) {
                // DEBUG
                if (isDebugChunk && x == 0 && z == 0) {
                  std::cout << "DEBUG: Sandstone Generated at y=" << y
                            << std::endl;
                }
                remainingDepth = random.nextInt(4);
                fillerBlock = 24; // SandStone
              }
            }
          }
        }
      }
    }
  }

  class SimpleWorldAdapter : public WorldInterface {
  public:
    std::vector<Chunk> &chunks;
    int radius;
    SimpleWorldAdapter(std::vector<Chunk> &c, int r) : chunks(c), radius(r) {}

    int getBlockId(int x, int y, int z) override {
      int cx = x >> 4;
      int cz = z >> 4;
      int lx = x & 15;
      int lz = z & 15;
      int gridWidth = radius * 2;
      int idx = (cx + radius) * gridWidth + (cz + radius);

      if (idx < 0 || idx >= chunks.size())
        return 0;
      if (chunks[idx].chunkX != cx || chunks[idx].chunkZ != cz)
        return 0;
      return chunks[idx].getBlock(lx, y, lz);
    }

    void setBlock(int x, int y, int z, int id) override {
      int cx = x >> 4;
      int cz = z >> 4;
      int lx = x & 15;
      int lz = z & 15;
      int gridWidth = radius * 2;
      int idx = (cx + radius) * gridWidth + (cz + radius);

      if (idx < 0 || idx >= chunks.size())
        return;
      if (chunks[idx].chunkX != cx || chunks[idx].chunkZ != cz)
        return;
      chunks[idx].setBlock(lx, y, lz, (uint8_t)id);
    }
  };

  void populate(std::vector<Chunk> &chunks, int radius, int chunkX,
                int chunkZ) {
    SimpleWorldAdapter world(chunks, radius);
    JavaRandom chunkRand(worldSeed);
    long xMul = chunkRand.nextLong() / 2 * 2 + 1;
    long zMul = chunkRand.nextLong() / 2 * 2 + 1;
    chunkRand.setSeed((long)chunkX * xMul + (long)chunkZ * zMul ^ worldSeed);

    int globeX = chunkX * 16;
    int globeZ = chunkZ * 16;

    for (int i = 0; i < 20; i++)
      WorldGenMinable(BlockID::CoalOre, 16)
          .generate(world, chunkRand, globeX + chunkRand.nextInt(16),
                    chunkRand.nextInt(128), globeZ + chunkRand.nextInt(16));
    for (int i = 0; i < 20; i++)
      WorldGenMinable(BlockID::IronOre, 8)
          .generate(world, chunkRand, globeX + chunkRand.nextInt(16),
                    chunkRand.nextInt(64), globeZ + chunkRand.nextInt(16));
    for (int i = 0; i < 2; i++)
      WorldGenMinable(BlockID::GoldOre, 8)
          .generate(world, chunkRand, globeX + chunkRand.nextInt(16),
                    chunkRand.nextInt(32), globeZ + chunkRand.nextInt(16));
    for (int i = 0; i < 8; i++)
      WorldGenMinable(BlockID::RedstoneOre, 7)
          .generate(world, chunkRand, globeX + chunkRand.nextInt(16),
                    chunkRand.nextInt(16), globeZ + chunkRand.nextInt(16));
    for (int i = 0; i < 1; i++)
      WorldGenMinable(BlockID::DiamondOre, 7)
          .generate(world, chunkRand, globeX + chunkRand.nextInt(16),
                    chunkRand.nextInt(16), globeZ + chunkRand.nextInt(16));
    for (int i = 0; i < 1; i++)
      WorldGenMinable(BlockID::LapisOre, 6)
          .generate(world, chunkRand, globeX + chunkRand.nextInt(16),
                    chunkRand.nextInt(16) + chunkRand.nextInt(16),
                    globeZ + chunkRand.nextInt(16));
    for (int i = 0; i < 20; i++)
      WorldGenMinable(BlockID::Dirt, 32)
          .generate(world, chunkRand, globeX + chunkRand.nextInt(16),
                    chunkRand.nextInt(128), globeZ + chunkRand.nextInt(16));
    for (int i = 0; i < 10; i++)
      WorldGenMinable(BlockID::Gravel, 32)
          .generate(world, chunkRand, globeX + chunkRand.nextInt(16),
                    chunkRand.nextInt(128), globeZ + chunkRand.nextInt(16));

    double temp, rain;
    biomeManager.getTempRain(globeX + 8, globeZ + 8, temp, rain);
    BiomeID biome = biomeManager.getBiomeFromValues(temp, rain);
    int treeCount = biomeManager.getTreesPerChunk(biome);
    if (chunkRand.nextInt(10) == 0)
      treeCount++;

    for (int i = 0; i < treeCount; ++i) {
      int x = globeX + chunkRand.nextInt(16) + 8;
      int z = globeZ + chunkRand.nextInt(16) + 8;
      int y = 127;
      while (y > 0 && world.getBlockId(x, y, z) == 0)
        y--;
      if (y > 0)
        WorldGenTrees().generate(world, chunkRand, x, y + 1, z);
    }
    // check biome at 0,0
    BiomeProperties b = biomeManager.getBiomeProps(0, 0);
    std::cout << "C++ Biome at 0,0 ID: " << (int)b.id << std::endl;
  }

  void generateChunk(class Chunk &chunk) {
    std::vector<double> densities =
        calculateDensity(chunk.chunkX, chunk.chunkZ);

    for (int x = 0; x < 4; ++x) {
      for (int z = 0; z < 4; ++z) {
        for (int y = 0; y < 16; ++y) {

          double d1 = densities[(x * 5 + z) * 17 + y];
          double d2 = densities[(x * 5 + z + 1) * 17 + y];
          double d3 = densities[((x + 1) * 5 + z) * 17 + y];
          double d4 = densities[((x + 1) * 5 + z + 1) * 17 + y];
          double d5 = densities[(x * 5 + z) * 17 + y + 1];
          double d6 = densities[(x * 5 + z + 1) * 17 + y + 1];
          double d7 = densities[((x + 1) * 5 + z) * 17 + y + 1];
          double d8 = densities[((x + 1) * 5 + z + 1) * 17 + y + 1];

          double dy1 = (d5 - d1) * 0.125;
          double dy2 = (d6 - d2) * 0.125;
          double dy3 = (d7 - d3) * 0.125;
          double dy4 = (d8 - d4) * 0.125;

          double val1 = d1;
          double val2 = d2;
          double val3 = d3;
          double val4 = d4;

          for (int i = 0; i < 8; ++i) {
            double dx1 = (val3 - val1) * 0.25;
            double dx2 = (val4 - val2) * 0.25;

            double valX1 = val1;
            double valX2 = val2;

            for (int j = 0; j < 4; ++j) {
              double dz = (valX2 - valX1) * 0.25;
              double valZ = valX1;

              for (int k = 0; k < 4; ++k) {
                int blockY = y * 8 + i;

                if (valZ > -0.0000001) {
                  chunk.setBlock(x * 4 + j, blockY, z * 4 + k, 1);
                } else if (blockY < 64) {
                  chunk.setBlock(x * 4 + j, blockY, z * 4 + k, 9);
                } else {
                  chunk.setBlock(x * 4 + j, blockY, z * 4 + k, 0);
                }

                valZ += dz;
              }
              valX1 += dx1;
              valX2 += dx2;
            }
            val1 += dy1;
            val2 += dy2;
            val3 += dy3;
            val4 += dy4;
          }
        }
      }
    }

    replaceBlocksForBiome(chunk);
    generateCaves(chunk);
  }

  void generateCaves(class Chunk &chunk) {
    int chunkX = chunk.chunkX;
    int chunkZ = chunk.chunkZ;
    int range = 8;

    JavaRandom baseRand(worldSeed);
    long r1 = baseRand.nextLong() / 2 * 2 + 1;
    long r2 = baseRand.nextLong() / 2 * 2 + 1;

    for (int cx = chunkX - range; cx <= chunkX + range; ++cx) {
      for (int cz = chunkZ - range; cz <= chunkZ + range; ++cz) {
        long chunkSeed = (long)cx * r1 + (long)cz * r2 ^ worldSeed;
        JavaRandom caveRand(chunkSeed);

        int numCaves =
            caveRand.nextInt(caveRand.nextInt(caveRand.nextInt(40) + 1) + 1);
        if (caveRand.nextInt(15) != 0)
          numCaves = 0;

        for (int i = 0; i < numCaves; ++i) {
          double startX = cx * 16 + caveRand.nextInt(16);
          double startY = caveRand.nextInt(caveRand.nextInt(120) + 8);
          double startZ = cz * 16 + caveRand.nextInt(16);

          int numNodes = 1;
          if (caveRand.nextInt(4) == 0) {
            generateCaveNode(caveRand.nextLong(), chunk, startX, startY, startZ,
                             1.0f + caveRand.nextFloat() * 6.0f);
            numNodes += caveRand.nextInt(4);
          }

          for (int j = 0; j < numNodes; ++j) {
            float yaw = caveRand.nextFloat() * 3.14159f * 2.0f;
            float pitch = (caveRand.nextFloat() - 0.5f) * 2.0f / 8.0f;
            float width = caveRand.nextFloat() * 2.0f + caveRand.nextFloat();
            generateCaveNode(caveRand.nextLong(), chunk, startX, startY, startZ,
                             width, yaw, pitch, 0, 0, 1.0);
          }
        }
      }
    }
  }

  void generateCaveNode(long seed, class Chunk &chunk, double x, double y,
                        double z, float width) {
    generateCaveNode(seed, chunk, x, y, z, width, 0.0f, 0.0f, -1, -1, 0.5);
  }

  void generateCaveNode(long seed, class Chunk &chunk, double targetX,
                        double targetY, double targetZ, float width, float yaw,
                        float pitch, int curStep, int numSteps, double yScale) {
    double centerX = chunk.chunkX * 16 + 8;
    double centerZ = chunk.chunkZ * 16 + 8;
    float yawVal = 0.0f;
    float pitchVal = 0.0f;
    JavaRandom nodeRand(seed);

    if (numSteps <= 0) {
      int stepRange = 8 * 16 - 16;
      numSteps = stepRange - nodeRand.nextInt(stepRange / 4);
    }

    bool isWide = false;
    if (curStep == -1) {
      curStep = numSteps / 2;
      isWide = true;
    }

    int targetStep = nodeRand.nextInt(numSteps / 2) + numSteps / 4;
    bool randomFlag = nodeRand.nextInt(6) == 0;

    for (; curStep < numSteps; ++curStep) {
      double widthAtStep =
          1.5 + MathHelper::sin((float)curStep * 3.14159f / (float)numSteps) *
                    width * 1.0f;
      double heightAtStep = widthAtStep * yScale;

      float cosPitch = MathHelper::cos(pitch);
      float sinPitch = MathHelper::sin(pitch);

      targetX += MathHelper::cos(yaw) * cosPitch;
      targetY += sinPitch;
      targetZ += MathHelper::sin(yaw) * cosPitch;

      if (randomFlag)
        pitch *= 0.92f;
      else
        pitch *= 0.7f;

      pitch += pitchVal * 0.1f;
      yaw += yawVal * 0.1f;
      pitchVal *= 0.9f;
      yawVal *= 0.75f;
      pitchVal += (nodeRand.nextFloat() - nodeRand.nextFloat()) *
                  nodeRand.nextFloat() * 2.0f;
      yawVal += (nodeRand.nextFloat() - nodeRand.nextFloat()) *
                nodeRand.nextFloat() * 4.0f;

      if (!isWide && curStep == targetStep && width > 1.0f && numSteps > 0) {
        generateCaveNode(nodeRand.nextLong(), chunk, targetX, targetY, targetZ,
                         nodeRand.nextFloat() * 0.5f + 0.5f, yaw - 1.57f,
                         pitch / 3.0f, curStep, numSteps, 1.0);
        generateCaveNode(nodeRand.nextLong(), chunk, targetX, targetY, targetZ,
                         nodeRand.nextFloat() * 0.5f + 0.5f, yaw + 1.57f,
                         pitch / 3.0f, curStep, numSteps, 1.0);
        return;
      }

      if (isWide || nodeRand.nextInt(4) != 0) {
        double dx = targetX - centerX;
        double dz = targetZ - centerZ;
        double stepsRem = numSteps - curStep;
        double maxDist = width + 2.0 + 16.0;

        if (dx * dx + dz * dz - stepsRem * stepsRem > maxDist * maxDist)
          return;

        if (targetX >= centerX - 16 - widthAtStep * 2 &&
            targetZ >= centerZ - 16 - widthAtStep * 2 &&
            targetX <= centerX + 16 + widthAtStep * 2 &&
            targetZ <= centerZ + 16 + widthAtStep * 2) {

          int minX =
              MathHelper::floor(targetX - widthAtStep) - chunk.chunkX * 16 - 1;
          int maxX =
              MathHelper::floor(targetX + widthAtStep) - chunk.chunkX * 16 + 1;
          int minY = MathHelper::floor(targetY - heightAtStep) - 1;
          int maxY = MathHelper::floor(targetY + heightAtStep) + 1;
          int minZ =
              MathHelper::floor(targetZ - widthAtStep) - chunk.chunkZ * 16 - 1;
          int maxZ =
              MathHelper::floor(targetZ + widthAtStep) - chunk.chunkZ * 16 + 1;

          if (minX < 0)
            minX = 0;
          if (maxX > 16)
            maxX = 16;
          if (minY < 1)
            minY = 1;
          if (maxY > 120)
            maxY = 120;
          if (minZ < 0)
            minZ = 0;
          if (maxZ > 16)
            maxZ = 16;

          bool hitWater = false;
          for (int bx = minX; !hitWater && bx < maxX; ++bx) {
            for (int bz = minZ; !hitWater && bz < maxZ; ++bz) {
              for (int by = maxY + 1; !hitWater && by >= minY - 1; --by) {
                int blk = chunk.getBlock(bx, by, bz);
                if (blk == 9 || blk == 8)
                  hitWater = true;
                if (by != minY - 1 && bx != minX && bx != maxX - 1 &&
                    bz != minZ && bz != maxZ - 1)
                  by = minY;
              }
            }
          }

          if (!hitWater) {
            for (int bx = minX; bx < maxX; ++bx) {
              double blockRelX =
                  ((double)(bx + chunk.chunkX * 16) + 0.5 - targetX) /
                  widthAtStep;
              for (int bz = minZ; bz < maxZ; ++bz) {
                double blockRelZ =
                    ((double)(bz + chunk.chunkZ * 16) + 0.5 - targetZ) /
                    widthAtStep;
                if (blockRelX * blockRelX + blockRelZ * blockRelZ < 1.0) {
                  for (int by = maxY - 1; by >= minY; --by) {
                    double blockRelY =
                        ((double)by + 0.5 - targetY) / heightAtStep;
                    if (blockRelY > -0.7 && blockRelX * blockRelX +
                                                    blockRelY * blockRelY +
                                                    blockRelZ * blockRelZ <
                                                1.0) {
                      int existing = chunk.getBlock(bx, by, bz);
                      if (existing == 1 || existing == 3 || existing == 2) {
                        if (by < 10)
                          chunk.setBlock(bx, by, bz, 11);
                        else {
                          chunk.setBlock(bx, by, bz, 0);
                          if (existing == 2 &&
                              chunk.getBlock(bx, by - 1, bz) == 3)
                            chunk.setBlock(bx, by - 1, bz, 2);
                        }
                      }
                    }
                  }
                }
              }
            }
            if (isWide)
              break;
          }
        }
      }
    }
  }
};