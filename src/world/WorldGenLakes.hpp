#pragma once
#include "../core/JavaRandom.hpp"
#include "Feature.hpp"
#include "MathHelper.hpp"
#include <cmath>
#include <vector>

class WorldGenLakes : public WorldGenerator {
private:
  int blockIndex;

  bool isMaterialSolid(int id) {
    return id != 0 && id != 8 && id != 9 && id != 10 && id != 11;
  }

  bool isMaterialFluid(int id) {
    return id == 8 || id == 9 || id == 10 || id == 11;
  }

public:
  WorldGenLakes(int blockId) : blockIndex(blockId) {}

  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y,
                int z) override {
    x -= 8;
    z -= 8;

    while (y > 0 && world.getBlockId(x, y, z) == 0) {
      y--;
    }

    y -= 4;
    y -= 4;

    if (y < 0)
      return false;

    bool shouldPlace[16][16][8];
    for (int i = 0; i < 16; ++i)
      for (int k = 0; k < 16; ++k)
        for (int j = 0; j < 8; ++j)
          shouldPlace[i][k][j] = false;

    int count = rand.nextInt(4) + 4;

    for (int i = 0; i < count; ++i) {
      double r1 = rand.nextDouble();
      double r2 = rand.nextDouble();
      double r3 = rand.nextDouble();

      double d = r1 * 6.0 + 3.0;
      double d1 = r2 * 4.0 + 2.0;
      double d2 = r3 * 6.0 + 3.0;

      double r4 = rand.nextDouble();
      double r5 = rand.nextDouble();
      double r6 = rand.nextDouble();

      double d3 = r4 * (16.0 - d - 2.0) + 1.0 + d / 2.0;
      double d4 = r5 * (8.0 - d1 - 4.0) + 2.0 + d1 / 2.0;
      double d5 = r6 * (16.0 - d2 - 2.0) + 1.0 + d2 / 2.0;

      for (int dx = 1; dx < 15; ++dx) {
        for (int dz = 1; dz < 15; ++dz) {
          for (int dy = 1; dy < 7; ++dy) {
            double d6 = ((double)dx - d3) / (d / 2.0);
            double d7 = ((double)dy - d4) / (d1 / 2.0);
            double d8 = ((double)dz - d5) / (d2 / 2.0);
            double distSq = d6 * d6 + d7 * d7 + d8 * d8;

            if (distSq < 1.0) {
              shouldPlace[dx][dz][dy] = true;
            }
          }
        }
      }
    }

    for (int dx = 0; dx < 16; ++dx) {
      for (int dz = 0; dz < 16; ++dz) {
        for (int dy = 0; dy < 8; ++dy) {
          bool flag = !shouldPlace[dx][dz][dy] &&
                      ((dx < 15 && shouldPlace[dx + 1][dz][dy]) ||
                       (dx > 0 && shouldPlace[dx - 1][dz][dy]) ||
                       (dz < 15 && shouldPlace[dx][dz + 1][dy]) ||
                       (dz > 0 && shouldPlace[dx][dz - 1][dy]) ||
                       (dy < 7 && shouldPlace[dx][dz][dy + 1]) ||
                       (dy > 0 && shouldPlace[dx][dz][dy - 1]));

          if (flag) {
            int materialId = world.getBlockId(x + dx, y + dy, z + dz);

            if (dy >= 4 && isMaterialFluid(materialId)) {
              return false;
            }
            if (dy < 4 && !isMaterialSolid(materialId) &&
                world.getBlockId(x + dx, y + dy, z + dz) != blockIndex) {
              return false;
            }
          }
        }
      }
    }

    for (int dx = 0; dx < 16; ++dx) {
      for (int dz = 0; dz < 16; ++dz) {
        for (int dy = 0; dy < 8; ++dy) {
          if (shouldPlace[dx][dz][dy]) {
            int toPlace = (dy >= 4) ? 0 : blockIndex;
            world.setBlock(x + dx, y + dy, z + dz, toPlace);
          }
        }
      }
    }

    for (int dx = 0; dx < 16; ++dx) {
      for (int dz = 0; dz < 16; ++dz) {
        for (int dy = 4; dy < 8; ++dy) {
          if (shouldPlace[dx][dz][dy]) {
            int checkX = x + dx;
            int checkY = y + dy - 1;
            int checkZ = z + dz;
            if (world.getBlockId(checkX, checkY, checkZ) == 3) {
              world.setBlock(checkX, checkY, checkZ, 2);
            }
          }
        }
      }
    }

    if (blockIndex == 10 || blockIndex == 11) {
      for (int dx = 0; dx < 16; ++dx) {
        for (int dz = 0; dz < 16; ++dz) {
          for (int dy = 0; dy < 8; ++dy) {

            bool flag = !shouldPlace[dx][dz][dy] &&
                        ((dx < 15 && shouldPlace[dx + 1][dz][dy]) ||
                         (dx > 0 && shouldPlace[dx - 1][dz][dy]) ||
                         (dz < 15 && shouldPlace[dx][dz + 1][dy]) ||
                         (dz > 0 && shouldPlace[dx][dz - 1][dy]) ||
                         (dy < 7 && shouldPlace[dx][dz][dy + 1]) ||
                         (dy > 0 && shouldPlace[dx][dz][dy - 1]));

            if (flag && (dy < 4 || rand.nextInt(2) != 0)) {
              int px = x + dx;
              int py = y + dy;
              int pz = z + dz;
              int matId = world.getBlockId(px, py, pz);

              if (isMaterialSolid(matId)) {
                world.setBlock(px, py, pz, 1);
              }
            }
          }
        }
      }
    }

    return true;
  }
};

class WorldGenLiquids : public WorldGenerator {
private:
  int liquidBlockId;

public:
  WorldGenLiquids(int id) : liquidBlockId(id) {}

  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y,
                int z) override {
    if (world.getBlockId(x, y + 1, z) != 1)
      return false;
    if (world.getBlockId(x, y - 1, z) != 1)
      return false;
    int current = world.getBlockId(x, y, z);
    if (current != 0 && current != 1)
      return false;

    int stoneCount = 0;
    int airCount = 0;

    int id = world.getBlockId(x + 1, y, z);
    if (id == 1)
      stoneCount++;
    else if (id == 0)
      airCount++;

    id = world.getBlockId(x - 1, y, z);
    if (id == 1)
      stoneCount++;
    else if (id == 0)
      airCount++;

    id = world.getBlockId(x, y, z + 1);
    if (id == 1)
      stoneCount++;
    else if (id == 0)
      airCount++;

    id = world.getBlockId(x, y, z - 1);
    if (id == 1)
      stoneCount++;
    else if (id == 0)
      airCount++;

    if (stoneCount == 3 && airCount == 1) {
      world.setBlock(x, y, z, liquidBlockId);
    }

    return true;
  }
};