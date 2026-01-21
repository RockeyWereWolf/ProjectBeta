#pragma once
#include "Feature.hpp"
#include "MathHelper.hpp"
#include <cmath>

class WorldGenMinable : public WorldGenerator {
private:
  int minableBlockId;
  int numberOfBlocks;
  int replaceBlockId;

public:
  WorldGenMinable(int blockId, int count)
      : minableBlockId(blockId), numberOfBlocks(count), replaceBlockId(1) {}

  WorldGenMinable(int blockId, int count, int targetId)
      : minableBlockId(blockId), numberOfBlocks(count),
        replaceBlockId(targetId) {}

  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y,
                int z) override {
    float f = rand.nextFloat() * MathHelper::MC_PI<float>();

    double d =
        (float)(x + 8) + MathHelper::sin(f) * (float)numberOfBlocks / 8.0f;
    double d1 =
        (float)(x + 8) - MathHelper::sin(f) * (float)numberOfBlocks / 8.0f;
    double d2 =
        (float)(z + 8) + MathHelper::cos(f) * (float)numberOfBlocks / 8.0f;
    double d3 =
        (float)(z + 8) - MathHelper::cos(f) * (float)numberOfBlocks / 8.0f;

    double d4 = y + rand.nextInt(3) + 2;
    double d5 = y + rand.nextInt(3) + 2;

    for (int i = 0; i <= numberOfBlocks; ++i) {
      double d6 = d + (d1 - d) * (double)i / (double)numberOfBlocks;
      double d7 = d4 + (d5 - d4) * (double)i / (double)numberOfBlocks;
      double d8 = d2 + (d3 - d2) * (double)i / (double)numberOfBlocks;
      double d9 = rand.nextDouble() * (double)numberOfBlocks / 16.0;

      double d10 =
          (double)(MathHelper::sin((float)i * MathHelper::MC_PI<float>() /
                                   (float)numberOfBlocks) +
                   1.0f) *
              d9 +
          1.0;

      double d11 =
          (double)(MathHelper::sin((float)i * MathHelper::MC_PI<float>() /
                                   (float)numberOfBlocks) +
                   1.0f) *
              d9 +
          1.0;

      int j = MathHelper::floor(d6 - d10 / 2.0);
      int k = MathHelper::floor(d7 - d11 / 2.0);
      int l = MathHelper::floor(d8 - d10 / 2.0);
      int i1 = MathHelper::floor(d6 + d10 / 2.0);
      int j1 = MathHelper::floor(d7 + d11 / 2.0);
      int k1 = MathHelper::floor(d8 + d10 / 2.0);

      for (int xPos = j; xPos <= i1; ++xPos) {
        double d12 = ((double)xPos + 0.5 - d6) / (d10 / 2.0);
        if (d12 * d12 < 1.0) {
          for (int yPos = k; yPos <= j1; ++yPos) {
            double d13 = ((double)yPos + 0.5 - d7) / (d11 / 2.0);
            if (d12 * d12 + d13 * d13 < 1.0) {
              for (int zPos = l; zPos <= k1; ++zPos) {
                double d14 = ((double)zPos + 0.5 - d8) / (d10 / 2.0);
                if (d12 * d12 + d13 * d13 + d14 * d14 < 1.0) {

                  int currentId = world.getBlockId(xPos, yPos, zPos);
                  if (currentId == replaceBlockId) {
                    world.setBlock(xPos, yPos, zPos, minableBlockId);
                  }
                }
              }
            }
          }
        }
      }
    }
    return true;
  }
};