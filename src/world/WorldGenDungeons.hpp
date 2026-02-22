// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * SPDX-FileCopyrightText: © 2026 RockeyWereWolf <werewolfoffers@protonmail.com>
 * SPDX-FileCopyrightText: © 2011 Mojang AB
 *
 * C++ implementation of dungeon generation.
 * For now serves as a simulation of RNG calls for chest loot and spawner
 * entities to maintain seed synchronization with the original game.
 *
 * References decompiled code from Retro-MCP.
 */

#pragma once
#include "../core/JavaRandom.hpp"
#include "Feature.hpp"

class WorldGenDungeons : public WorldGenerator {
private:
  bool isSolid(WorldInterface &world, int x, int y, int z) {
    int id = world.getBlockId(x, y, z);
    if (id == 0 || id == 8 || id == 9 || id == 10 || id == 11)
      return false;
    return true;
  }

  void genChestLoot(JavaRandom &rand) {
    int pick = rand.nextInt(11);
    // simulating loot rng calls
    switch (pick) {
    case 0: // Saddle
      break;
    case 1:            // Iron Ingot
      rand.nextInt(4); // Stack size
      break;
    case 2: // Bread
      break;
    case 3: // Bread
      break;
    case 4: // Gunpowder
      rand.nextInt(4);
      break;
    case 5: // String
      rand.nextInt(4);
      break;
    case 6: // Bucket
      break;
    case 7: // Golden Apple
      if (rand.nextInt(100) == 0) {
      }
      break;
    case 8: // Redstone
      if (rand.nextInt(2) == 0) {
        rand.nextInt(4);
      }
      break;
    case 9: // Records
      if (rand.nextInt(10) == 0) {
        rand.nextInt(2);
      }
      break;
    case 10: // Dye
      break;
    }
  }

  void genSpawnerEntity(JavaRandom &rand) { rand.nextInt(4); }

public:
  WorldGenDungeons() {}

  bool generate(WorldInterface &world, JavaRandom &rand, int x, int y,
                int z) override {
    int xRadius = rand.nextInt(2) + 2;
    int zRadius = rand.nextInt(2) + 2;
    int height = 3;
    int startX = x - xRadius - 1;
    int startY = y - 1;
    int startZ = z - zRadius - 1;

    int endX = x + xRadius + 1;
    int endY = y + height + 1;
    int endZ = z + zRadius + 1;

    int airCount = 0;

    for (int dx = startX; dx <= endX; ++dx) {
      for (int dy = startY; dy <= endY; ++dy) {
        for (int dz = startZ; dz <= endZ; ++dz) {

          bool solid = isSolid(world, dx, dy, dz);

          if (dy == startY && !solid)
            return false;

          if (dy == endY && !solid)
            return false;

          if (dy == y &&
              (dx == startX || dx == endX || dz == startZ || dz == endZ)) {
            if (world.getBlockId(dx, dy, dz) == 0 &&
                world.getBlockId(dx, dy + 1, dz) == 0) {
              airCount++;
            }
          }
        }
      }
    }

    if (airCount < 1 || airCount > 5) {
      return false;
    }

    for (int dx = startX; dx <= endX; ++dx) {
      for (int dy = endY - 1; dy >= startY; --dy) {
        for (int dz = startZ; dz <= endZ; ++dz) {

          int currentId = world.getBlockId(dx, dy, dz);

          if (dx != startX && dy != startY && dz != startZ && dx != endX &&
              dz != endZ) {
            world.setBlock(dx, dy, dz, 0);
          } else if (dy >= 0 && !isSolid(world, dx, dy - 1, dz)) {
            world.setBlock(dx, dy, dz, 0);
          } else if (isSolid(world, dx, dy, dz)) {
            if (dy == startY && rand.nextInt(4) != 0) {
              world.setBlock(dx, dy, dz, 48);
            } else {
              world.setBlock(dx, dy, dz, 4);
            }
          }
        }
      }
    }

    for (int i = 0; i < 2; ++i) {
      for (int attempt = 0; attempt < 3; ++attempt) {
        int cx = x + rand.nextInt(xRadius * 2 + 1) - xRadius;
        int cy = y;
        int cz = z + rand.nextInt(zRadius * 2 + 1) - zRadius;

        if (world.getBlockId(x, y, z) == 0) {

          int solidNeighbors = 0;
          if (isSolid(world, cx + 1, cy, cz))
            solidNeighbors++;
          if (isSolid(world, cx - 1, cy, cz))
            solidNeighbors++;
          if (isSolid(world, cx, cy, cz + 1))
            solidNeighbors++;
          if (isSolid(world, cx, cy, cz - 1))
            solidNeighbors++;

          if (solidNeighbors == 1) {
            world.setBlock(cx, cy, cz, 54);

            // call loot gen
            for (int k = 0; k < 8; ++k) {
              genChestLoot(rand);
            }

            break;
          }
        }
      }
    }

    world.setBlock(x, y, z, 52);
    genSpawnerEntity(rand);

    return true;
  }
};