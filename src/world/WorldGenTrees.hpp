#pragma once
#include "../core/JavaRandom.hpp"
#include "Chunk.hpp"
#include "Feature.hpp"
#include <cmath>
#include <cstdlib>

inline bool isOpaque(int id) { return id != 0 && id != BlockID::Leaves; }

class WorldGenTrees : public WorldGenerator {
public:
  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y, int z) {
    int height = rand.nextInt(3) + 4;
    bool canGrow = true;

    if (y < 1 || y + height + 1 > 128)
      return false;

    for (int i = y; i <= y + 1 + height; ++i) {
      int radius = 1;
      if (i == y)
        radius = 0;
      if (i >= y + 1 + height - 2)
        radius = 2;

      for (int k = x - radius; k <= x + radius && canGrow; ++k) {
        for (int l = z - radius; l <= z + radius && canGrow; ++l) {
          if (i >= 0 && i < 128) {
            int id = world.getBlockId(k, i, l);
            if (id != 0 && id != BlockID::Leaves)
              canGrow = false;
          } else {
            canGrow = false;
          }
        }
      }
    }

    if (!canGrow)
      return false;

    int soil = world.getBlockId(x, y - 1, z);
    if ((soil == BlockID::Grass || soil == BlockID::Dirt) &&
        y < 128 - height - 1) {
      world.setBlock(x, y - 1, z, BlockID::Dirt);

      for (int i = y - 3 + height; i <= y + height; ++i) {
        int distTop = i - (y + height);
        int radius = 1 - distTop / 2;

        for (int k = x - radius; k <= x + radius; ++k) {
          int dx = k - x;
          for (int l = z - radius; l <= z + radius; ++l) {
            int dz = l - z;
            if ((std::abs(dx) != radius || std::abs(dz) != radius ||
                 (rand.nextInt(2) != 0 && distTop != 0)) &&
                !isOpaque(world.getBlockId(k, i, l))) {
              world.setBlock(k, i, l, BlockID::Leaves);
            }
          }
        }
      }

      for (int i = 0; i < height; ++i) {
        int id = world.getBlockId(x, y + i, z);
        if (id == 0 || id == BlockID::Leaves)
          world.setBlock(x, y + i, z, BlockID::Log);
      }
      return true;
    }
    return false;
  }
};

class WorldGenForest {
public:
  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y, int z) {
    int height = rand.nextInt(3) + 5;
    bool canGrow = true;

    if (y < 1 || y + height + 1 > 128)
      return false;

    for (int i = y; i <= y + 1 + height; ++i) {
      int radius = 1;
      if (i == y)
        radius = 0;
      if (i >= y + 1 + height - 2)
        radius = 2;

      for (int k = x - radius; k <= x + radius && canGrow; ++k) {
        for (int l = z - radius; l <= z + radius && canGrow; ++l) {
          if (i >= 0 && i < 128) {
            int id = world.getBlockId(k, i, l);
            if (id != 0 && id != BlockID::Leaves)
              canGrow = false;
          } else {
            canGrow = false;
          }
        }
      }
    }

    if (!canGrow)
      return false;

    int soil = world.getBlockId(x, y - 1, z);
    if ((soil == BlockID::Grass || soil == BlockID::Dirt) &&
        y < 128 - height - 1) {
      world.setBlock(x, y - 1, z, BlockID::Dirt);

      for (int i = y - 3 + height; i <= y + height; ++i) {
        int distTop = i - (y + height);
        int radius = 1 - distTop / 2;

        for (int k = x - radius; k <= x + radius; ++k) {
          int dx = k - x;
          for (int l = z - radius; l <= z + radius; ++l) {
            int dz = l - z;
            if ((std::abs(dx) != radius || std::abs(dz) != radius ||
                 (rand.nextInt(2) != 0 && distTop != 0)) &&
                !isOpaque(world.getBlockId(k, i, l))) {
              world.setBlock(k, i, l,
                             BlockID::Leaves); // Birch Leaves (No meta yet)
            }
          }
        }
      }

      for (int i = 0; i < height; ++i) {
        int id = world.getBlockId(x, y + i, z);
        if (id == 0 || id == BlockID::Leaves) {
          world.setBlock(x, y + i, z, BlockID::Log); // Birch Wood (No meta yet)
        }
      }
      return true;
    }
    return false;
  }
};

class WorldGenTaiga2 {
public:
  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y, int z) {
    int height = rand.nextInt(4) + 6;
    int crownStart = 1 + rand.nextInt(2);
    int foliageHeight = height - crownStart;
    int layerLimit = 2 + rand.nextInt(2);
    bool canGrow = true;

    if (y < 1 || y + height + 1 > 128)
      return false;

    for (int i = y; i <= y + 1 + height && canGrow; ++i) {
      bool inBounds = true;
      int radius;
      if (i - y < crownStart)
        radius = 0;
      else
        radius = layerLimit;

      for (int k = x - radius; k <= x + radius && canGrow; ++k) {
        for (int l = z - radius; l <= z + radius && canGrow; ++l) {
          if (i >= 0 && i < 128) {
            int id = world.getBlockId(k, i, l);
            if (id != 0 && id != BlockID::Leaves)
              canGrow = false;
          } else
            canGrow = false;
        }
      }
    }

    if (!canGrow)
      return false;

    int soil = world.getBlockId(x, y - 1, z);
    if ((soil == BlockID::Grass || soil == BlockID::Dirt) &&
        y < 128 - height - 1) {
      world.setBlock(x, y - 1, z, BlockID::Dirt);

      int currentMaxRadius = rand.nextInt(2);
      int currentRadius = 1;
      uint8_t layerVar = 0;

      for (int i = 0; i <= foliageHeight; ++i) {
        int yLevel = y + height - i;
        for (int k = x - currentMaxRadius; k <= x + currentMaxRadius; ++k) {
          int dx = k - x;
          for (int l = z - currentMaxRadius; l <= z + currentMaxRadius; ++l) {
            int dz = l - z;
            if ((std::abs(dx) != currentMaxRadius ||
                 std::abs(dz) != currentMaxRadius || currentMaxRadius <= 0) &&
                !isOpaque(world.getBlockId(k, yLevel, l))) {
              world.setBlock(k, yLevel, l, BlockID::Leaves);
            }
          }
        }

        if (currentMaxRadius >= currentRadius) {
          currentMaxRadius = layerVar;
          layerVar = 1;
          currentRadius++;
          if (currentRadius > layerLimit)
            currentRadius = layerLimit;
        } else {
          currentMaxRadius++;
        }
      }

      int bareTrunkHeight = rand.nextInt(3);
      for (int i = 0; i < height - bareTrunkHeight; ++i) {
        int id = world.getBlockId(x, y + i, z);
        if (id == 0 || id == BlockID::Leaves) {
          world.setBlock(x, y + i, z, BlockID::Log);
        }
      }

      return true;
    }
    return false;
  }
};
