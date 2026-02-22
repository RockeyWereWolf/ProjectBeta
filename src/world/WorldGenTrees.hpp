// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * SPDX-FileCopyrightText: © 2026 RockeyWereWolf <werewolfoffers@protonmail.com>
 * SPDX-FileCopyrightText: © 2011 Mojang AB
 *
 * C++ implementation of tree generators.
 * Contains generators for: SimpleTrees, Spruces, and BigTrees.
 *
 * References decompiled code from Retro-MCP.
 */

#pragma once
#include "../core/JavaRandom.hpp"
#include "Chunk.hpp"
#include "Feature.hpp"
#include "MathHelper.hpp"
#include <cmath>
#include <vector>

class TreeUtils {
public:
  static bool checkTree(WorldInterface &world, int x, int y, int z, int height,
                        int (*radiusFunc)(int, int)) {
    int maxY = y + height + 1;
    if (y < 1 || maxY >= 128)
      return false;

    int soil = world.getBlockId(x, y - 1, z);
    if (soil != BlockID::Grass && soil != BlockID::Dirt)
      return false;

    for (int cy = y; cy <= maxY; ++cy) {
      int radius = radiusFunc(cy, y);
      for (int cx = x - radius; cx <= x + radius; ++cx) {
        for (int cz = z - radius; cz <= z + radius; ++cz) {
          int id = world.getBlockId(cx, cy, cz);
          if (id != 0 && id != BlockID::Leaves) {
            return false;
          }
        }
      }
    }
    return true;
  }

  static bool canGrowInto(WorldInterface &world, int x, int y, int z) {
    int id = world.getBlockId(x, y, z);
    return (id == 0 || id == BlockID::Leaves);
  }
};

class SimpleTreeGenerator : public WorldGenerator {
private:
  int minHeight;
  int metadata;

  static int checkRadius(int y, int baseY, int height) {
    if (y == baseY)
      return 0;
    if (y >= baseY + height - 1)
      return 2;
    return 1;
  }

public:
  SimpleTreeGenerator(int h, int meta) : minHeight(h), metadata(meta) {}

  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y,
                int z) override {
    int height = rand.nextInt(3) + minHeight;

    int maxY = y + height + 1;
    if (y < 1 || maxY >= 128)
      return false;
    int soil = world.getBlockId(x, y - 1, z);
    if (soil != BlockID::Grass && soil != BlockID::Dirt)
      return false;

    for (int cy = y; cy <= maxY; ++cy) {
      int r = checkRadius(cy, y, height);
      for (int cx = x - r; cx <= x + r; ++cx) {
        for (int cz = z - r; cz <= z + r; ++cz) {
          int id = world.getBlockId(cx, cy, cz);
          if (id != 0 && id != BlockID::Leaves)
            return false;
        }
      }
    }

    world.setBlock(x, y - 1, z, BlockID::Dirt, 0);

    for (int cy = y + height - 3; cy <= y + height; ++cy) {
      int dy = cy - (y + height);
      int radius = 1 - dy / 2;

      for (int cx = x - radius; cx <= x + radius; ++cx) {
        for (int cz = z - radius; cz <= z + radius; ++cz) {
          int dx = std::abs(cx - x);
          int dz = std::abs(cz - z);

          if (dx != radius || dz != radius ||
              (rand.nextInt(2) != 0 && dy != 0)) {
            if (TreeUtils::canGrowInto(world, cx, cy, cz)) {
              world.setBlock(cx, cy, cz, BlockID::Leaves, metadata);
            }
          }
        }
      }
    }

    for (int cy = y; cy < y + height; ++cy) {
      int id = world.getBlockId(x, cy, z);
      if (id == 0 || id == BlockID::Leaves) {
        world.setBlock(x, cy, z, BlockID::Log, metadata);
      }
    }

    return true;
  }
};

class Spruce1TreeGenerator : public WorldGenerator {
public:
  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y,
                int z) override {
    int height = rand.nextInt(5) + 7;
    int leavesOffset = height - rand.nextInt(2) - 3;
    int leavesHeight = height - leavesOffset;
    int maxRadius = rand.nextInt(leavesHeight + 1);
    int leavesY = y + leavesOffset;

    int maxY = y + height + 1;
    if (y < 1 || maxY >= 128)
      return false;
    if (world.getBlockId(x, y - 1, z) != BlockID::Grass &&
        world.getBlockId(x, y - 1, z) != BlockID::Dirt)
      return false;

    for (int cy = y; cy <= maxY; ++cy) {
      int r = (cy < leavesY) ? 0 : maxRadius;
      for (int cx = x - r; cx <= x + r; ++cx) {
        for (int cz = z - r; cz <= z + r; ++cz) {
          int id = world.getBlockId(cx, cy, cz);
          if (id != 0 && id != BlockID::Leaves)
            return false;
        }
      }
    }

    world.setBlock(x, y - 1, z, BlockID::Dirt, 0);

    int currentRadius = 0;
    for (int cy = leavesY; cy <= y + height; ++cy) {
      for (int cx = x - currentRadius; cx <= x + currentRadius; ++cx) {
        for (int cz = z - currentRadius; cz <= z + currentRadius; ++cz) {
          int dx = std::abs(cx - x);
          int dz = std::abs(cz - z);
          if (dx != currentRadius || dz != currentRadius ||
              currentRadius <= 0) {
            if (TreeUtils::canGrowInto(world, cx, cy, cz)) {
              world.setBlock(cx, cy, cz, BlockID::Leaves, 1);
            }
          }
        }
      }
      if (currentRadius >= 1 && cy == leavesY + 1)
        currentRadius -= 1;
      else if (currentRadius < maxRadius)
        currentRadius += 1;
    }

    for (int cy = y; cy < y + height - 1; ++cy) {
      int id = world.getBlockId(x, cy, z);
      if (id == 0 || id == BlockID::Leaves) {
        world.setBlock(x, cy, z, BlockID::Log, 1);
      }
    }
    return true;
  }
};

class Spruce2TreeGenerator : public WorldGenerator {
public:
  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y,
                int z) override {
    int height = rand.nextInt(4) + 6;
    int leavesOffset = rand.nextInt(2) + 1;
    int leavesHeight = height - leavesOffset;
    int maxRadius = rand.nextInt(2) + 2;
    int leavesY = y + leavesOffset;

    int maxY = y + height + 1;
    if (y < 1 || maxY >= 128)
      return false;
    if (world.getBlockId(x, y - 1, z) != BlockID::Grass &&
        world.getBlockId(x, y - 1, z) != BlockID::Dirt)
      return false;

    for (int cy = y; cy <= maxY; ++cy) {
      int r = (cy < leavesY) ? 0 : maxRadius;
      for (int cx = x - r; cx <= x + r; ++cx) {
        for (int cz = z - r; cz <= z + r; ++cz) {
          int id = world.getBlockId(cx, cy, cz);
          if (id != 0 && id != BlockID::Leaves)
            return false;
        }
      }
    }

    world.setBlock(x, y - 1, z, BlockID::Dirt, 0);

    int currentRadius = rand.nextInt(2);
    int startRadius = 0;
    int globalRadius = 1;

    for (int dy = 0; dy <= leavesHeight; ++dy) {
      int cy = y + height - dy;
      for (int cx = x - currentRadius; cx <= x + currentRadius; ++cx) {
        for (int cz = z - currentRadius; cz <= z + currentRadius; ++cz) {
          int dx = std::abs(cx - x);
          int dz = std::abs(cz - z);
          if (dx != currentRadius || dz != currentRadius ||
              currentRadius <= 0) {
            if (TreeUtils::canGrowInto(world, cx, cy, cz)) {
              world.setBlock(cx, cy, cz, BlockID::Leaves, 1);
            }
          }
        }
      }
      if (currentRadius >= globalRadius) {
        currentRadius = startRadius;
        startRadius = 1;
        globalRadius =
            (maxRadius < globalRadius + 1) ? maxRadius : globalRadius + 1;
      } else {
        currentRadius++;
      }
    }

    int logOffset = rand.nextInt(3);
    for (int cy = y; cy < y + height - logOffset; ++cy) {
      int id = world.getBlockId(x, cy, z);
      if (id == 0 || id == BlockID::Leaves) {
        world.setBlock(x, cy, z, BlockID::Log, 1);
      }
    }
    return true;
  }
};

struct BigTreeNode {
  int x, y, z;
  int startY;
};

class BigTreeGenerator : public WorldGenerator {
private:
  int heightRange = 12;
  float heightAttenuation = 0.618f;
  float leafDensity = 1.0f;
  int branchDeltaHeight = 4;
  float branchScale = 1.0f;
  float branchSlope = 0.381f;

  float calcLayerSize(int leafOffset, int height) {
    if ((float)leafOffset < (float)height * 0.3f)
      return -1.618f;
    float a = (float)height / 2.0f;
    float b = a - (float)leafOffset;
    float res;
    if (b == 0.0f)
      res = a;
    else if (std::abs(b) >= a)
      res = 0.0f;
    else
      res = std::sqrt(std::abs(a * a) - std::abs(b * b));
    return res * 0.5f;
  }

  void getStepLogic(int x1, int y1, int z1, int x2, int y2, int z2, int &steps,
                    float &d2, float &d3, int &incMajor, int &maxDim) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int dz = z2 - z1;

    maxDim = 0;
    int absDx = std::abs(dx);
    int absDy = std::abs(dy);
    int absDz = std::abs(dz);

    if (absDy > absDx && absDy > absDz)
      maxDim = 1;
    else if (absDz > absDx)
      maxDim = 2;

    if (maxDim == 0) {
      steps = absDx;
      incMajor = (dx > 0 ? 1 : -1);
      d2 = (float)dy / (float)absDx;
      d3 = (float)dz / (float)absDx;
    } else if (maxDim == 1) {
      steps = absDy;
      incMajor = (dy > 0 ? 1 : -1);
      d2 = (float)dx / (float)absDy;
      d3 = (float)dz / (float)absDy;
    } else {
      steps = absDz;
      incMajor = (dz > 0 ? 1 : -1);
      d2 = (float)dx / (float)absDz;
      d3 = (float)dy / (float)absDz;
    }
  }

  bool checkBranch(WorldInterface &world, int x1, int y1, int z1, int x2,
                   int y2, int z2) {
    int steps, incMajor, maxDim;
    float d2, d3;
    getStepLogic(x1, y1, z1, x2, y2, z2, steps, d2, d3, incMajor, maxDim);

    for (int i = 0; i <= steps; ++i) {
      int cx, cy, cz;
      if (maxDim == 0) {
        cx = x1 + i * incMajor;
        cy = MathHelper::floor((float)y1 + (float)i * d2 + 0.5f);
        cz = MathHelper::floor((float)z1 + (float)i * d3 + 0.5f);
      } else if (maxDim == 1) {
        cy = y1 + i * incMajor;
        cx = MathHelper::floor((float)x1 + (float)i * d2 + 0.5f);
        cz = MathHelper::floor((float)z1 + (float)i * d3 + 0.5f);
      } else {
        cz = z1 + i * incMajor;
        cx = MathHelper::floor((float)x1 + (float)i * d2 + 0.5f);
        cy = MathHelper::floor((float)y1 + (float)i * d3 + 0.5f);
      }

      int id = world.getBlockId(cx, cy, cz);
      if (id != 0 && id != BlockID::Leaves)
        return false;
    }
    return true;
  }

  void placeBranch(WorldInterface &world, int x1, int y1, int z1, int x2,
                   int y2, int z2) {
    int steps, incMajor, maxDim;
    float d2, d3;
    getStepLogic(x1, y1, z1, x2, y2, z2, steps, d2, d3, incMajor, maxDim);

    for (int i = 0; i <= steps; ++i) {
      int cx, cy, cz;
      if (maxDim == 0) {
        cx = x1 + i * incMajor;
        cy = MathHelper::floor((float)y1 + (float)i * d2 + 0.5f);
        cz = MathHelper::floor((float)z1 + (float)i * d3 + 0.5f);
      } else if (maxDim == 1) {
        cy = y1 + i * incMajor;
        cx = MathHelper::floor((float)x1 + (float)i * d2 + 0.5f);
        cz = MathHelper::floor((float)z1 + (float)i * d3 + 0.5f);
      } else {
        cz = z1 + i * incMajor;
        cx = MathHelper::floor((float)x1 + (float)i * d2 + 0.5f);
        cy = MathHelper::floor((float)y1 + (float)i * d3 + 0.5f);
      }
      world.setBlock(cx, cy, cz, BlockID::Log, 0);
    }
  }

  void placeLeafLayer(WorldInterface &world, int x, int y, int z,
                      float radius) {
    int r = (int)(radius + 0.618f);
    for (int dx = -r; dx <= r; ++dx) {
      for (int dz = -r; dz <= r; ++dz) {
        float dist = std::sqrt(std::pow(std::abs(dx) + 0.5f, 2.0f) +
                               std::pow(std::abs(dz) + 0.5f, 2.0f));
        if (dist <= radius) {
          if (TreeUtils::canGrowInto(world, x + dx, y, z + dz)) {
            world.setBlock(x + dx, y, z + dz, BlockID::Leaves, 0);
          }
        }
      }
    }
  }

public:
  static BigTreeGenerator newNatural() {
    BigTreeGenerator g;
    g.branchDeltaHeight = 5;
    return g;
  }

  bool generate(WorldInterface &world, JavaRandom &randIn, int x, int y,
                int z) override {
    long s = randIn.nextLong();
    JavaRandom rand(s);

    int height = rand.nextInt(heightRange) + 5;
    if (world.getBlockId(x, y - 1, z) != BlockID::Grass &&
        world.getBlockId(x, y - 1, z) != BlockID::Dirt)
      return false;

    if (!checkBranch(world, x, y, z, x, y + height, z))
      return false;

    int hAtt = (int)((float)height * heightAttenuation);
    if (hAtt >= height)
      hAtt = height - 1;
    int startY = y + hAtt;

    std::vector<BigTreeNode> nodes;
    int nodesPerHeight =
        (int)(1.382f + std::pow(leafDensity * (float)height / 13.0f, 2.0f));
    if (nodesPerHeight < 1)
      nodesPerHeight = 1;

    int leafOffset = height - branchDeltaHeight;
    int leafY = y + leafOffset;
    nodes.push_back({x, leafY, z, startY});
    leafY--;

    while (leafOffset >= 0) {
      float size = calcLayerSize(leafOffset, height);
      if (size >= 0.0f) {
        for (int i = 0; i < nodesPerHeight; ++i) {
          float len = branchScale * size * (rand.nextFloat() + 0.328f);
          float ang = rand.nextFloat() * 2.0f * MathHelper::MC_PI<float>();

          int lx = MathHelper::floor(len * std::sin(ang) + (float)x + 0.5f);
          int lz = MathHelper::floor(len * std::cos(ang) + (float)z + 0.5f);

          if (checkBranch(world, x, leafY, z, lx, leafY, lz) &&
              checkBranch(world, x, leafY + branchDeltaHeight, z, lx,
                          leafY + branchDeltaHeight, lz)) {

            float dist = std::sqrt(std::pow((float)(x - lx), 2.0f) +
                                   std::pow((float)(z - lz), 2.0f));
            int lStartY = (int)((float)leafY - dist * branchSlope);

            if (lStartY > startY)
              lStartY = startY;

            if (checkBranch(world, x, lStartY, z, lx, leafY, lz)) {
              nodes.push_back({lx, leafY, lz, lStartY});
            }
          }
        }
      }
      leafY--;
      leafOffset--;
    }

    for (const auto &n : nodes) {
      for (int dy = 0; dy < branchDeltaHeight; ++dy) {
        float r = (dy != 0 && dy != branchDeltaHeight - 1) ? 3.0f : 2.0f;
        placeLeafLayer(world, n.x, n.y + dy, n.z, r);
      }
    }

    placeBranch(world, x, y, z, x, y + hAtt, z);
    float minH = (float)height * 0.2f;
    for (const auto &n : nodes) {
      if ((float)(n.startY - y) >= minH) {
        placeBranch(world, x, n.startY, z, n.x, n.y, n.z);
      }
    }

    return true;
  }
};

class WorldGenTrees {
public:
  static WorldGenerator *newOak() { return new SimpleTreeGenerator(4, 0); }
  static WorldGenerator *newBirch() { return new SimpleTreeGenerator(5, 2); }
  static WorldGenerator *newSpruce1() { return new Spruce1TreeGenerator(); }
  static WorldGenerator *newSpruce2() { return new Spruce2TreeGenerator(); }
  static WorldGenerator *newBigNatural() {
    static BigTreeGenerator gen = BigTreeGenerator::newNatural();
    return &gen;
  }
  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y, int z) {
    SimpleTreeGenerator gen(4, 0);
    return gen.generate(world, rand, x, y, z);
  }
};