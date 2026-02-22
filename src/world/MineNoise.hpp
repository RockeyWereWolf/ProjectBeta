// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * SPDX-FileCopyrightText: © 2026 RockeyWereWolf <werewolfoffers@protonmail.com>
 * SPDX-FileCopyrightText: © 2011 Mojang AB
 *
 * C++ implementation of original game Perlin noise.
 *
 * References decompiled code from Retro-MCP.
 */

#pragma once
#include "../core/JavaRandom.hpp"
#include <vector>

// port of NoiseGeneratorPerlin
class MinePerlin {
private:
  int permutations[512];
  double xCoord;
  double yCoord;
  double zCoord;

  double lerp(double t, double a, double b) { return a + t * (b - a); }

  double func_4110_a(int hash, double x, double z) {
    int h = hash & 15;
    double u = (1 - ((h & 8) >> 3)) * x;
    double v = h < 4 ? 0.0 : (h != 12 && h != 14 ? z : x);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
  }

  double grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : (h != 12 && h != 14 ? z : x);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
  }

public:
  MinePerlin(JavaRandom &rand) {
    xCoord = rand.nextDouble() * 256.0;
    yCoord = rand.nextDouble() * 256.0;
    zCoord = rand.nextDouble() * 256.0;

    for (int i = 0; i < 256; ++i) {
      permutations[i] = i;
    }

    for (int i = 0; i < 256; ++i) {
      int j = rand.nextInt(256 - i) + i;
      int temp = permutations[i];
      permutations[i] = permutations[j];
      permutations[j] = temp;
      permutations[i + 256] = permutations[i];
    }
  }

  //  func_805_a
  void generate(double *arr, double x, double y, double z, int xSize, int ySize,
                int zSize, double xScale, double yScale, double zScale,
                double amp) {
    int index = 0;
    double invAmp = 1.0 / amp;
    int lastY = -1;

    // Optimizations variables
    double lerp0, lerp1, lerp2, lerp3;

    if (ySize == 1) {
    }
    {

      for (int ix = 0; ix < xSize; ++ix) {
        double dX = (x + ix) * xScale + xCoord;
        int floorX = (int)dX;
        if (dX < floorX)
          floorX--;
        int iX = floorX & 255;
        dX -= floorX;
        double fadeX = dX * dX * dX * (dX * (dX * 6.0 - 15.0) + 10.0);

        for (int iz = 0; iz < zSize; ++iz) {
          double dZ = (z + iz) * zScale + zCoord;
          int floorZ = (int)dZ;
          if (dZ < floorZ)
            floorZ--;
          int iZ = floorZ & 255;
          dZ -= floorZ;
          double fadeZ = dZ * dZ * dZ * (dZ * (dZ * 6.0 - 15.0) + 10.0);

          for (int iy = 0; iy < ySize; ++iy) {
            double dY = (y + iy) * yScale + yCoord;
            int floorY = (int)dY;
            if (dY < floorY)
              floorY--;
            int iY = floorY & 255;
            dY -= floorY;
            double fadeY = dY * dY * dY * (dY * (dY * 6.0 - 15.0) + 10.0);

            if (iy == 0 || iY != lastY) {
              lastY = iY;

              int A = permutations[iX] + iY;     // var69
              int AA = permutations[A] + iZ;     // var71
              int AB = permutations[A + 1] + iZ; // var72
              int B = permutations[iX + 1] + iY; // var74
              int BA = permutations[B] + iZ;     // var75
              int BB = permutations[B + 1] + iZ; // var76

              // var29: lerp(fadeX, grad(p[AA], ...), grad(p[BA], ...))
              // var31: lerp(fadeX, grad(p[AB], ...), grad(p[BB], ...)) (dY-1
              // case) var33: lerp(fadeX, grad(p[AA+1], ...), grad(p[BA+1],
              // ...)) (dZ-1 case) var35: lerp(fadeX, grad(p[AB+1], ...),
              // grad(p[BB+1], ...)) (dY-1, dZ-1 case)

              lerp0 = lerp(fadeX, grad(permutations[AA], dX, dY, dZ),
                           grad(permutations[BA], dX - 1, dY, dZ));
              lerp1 = lerp(fadeX, grad(permutations[AB], dX, dY - 1, dZ),
                           grad(permutations[BB], dX - 1, dY - 1, dZ));
              lerp2 = lerp(fadeX, grad(permutations[AA + 1], dX, dY, dZ - 1),
                           grad(permutations[BA + 1], dX - 1, dY, dZ - 1));
              lerp3 =
                  lerp(fadeX, grad(permutations[AB + 1], dX, dY - 1, dZ - 1),
                       grad(permutations[BB + 1], dX - 1, dY - 1, dZ - 1));
            }

            double val1 = lerp(fadeY, lerp0, lerp1);
            double val2 = lerp(fadeY, lerp2, lerp3);
            double res = lerp(fadeZ, val1, val2);

            arr[index++] += res * invAmp;
          }
        }
      }
    }
  }
};

class MineOctaves {
private:
  std::vector<MinePerlin> generators;
  int octaves;

public:
  MineOctaves(JavaRandom &rand, int octaves) : octaves(octaves) {
    for (int i = 0; i < octaves; ++i) {
      generators.emplace_back(rand);
    }
  }

  void generate(double *arr, double x, double y, double z, int xSize, int ySize,
                int zSize, double xScale, double yScale, double zScale) {
    int size = xSize * ySize * zSize;
    for (int i = 0; i < size; ++i)
      arr[i] = 0.0;

    double amp = 1.0;
    for (int i = 0; i < octaves; ++i) {
      generators[i].generate(arr, x, y, z, xSize, ySize, zSize, xScale * amp,
                             yScale * amp, zScale * amp, amp);
      amp /= 2.0;
    }
  }
};
