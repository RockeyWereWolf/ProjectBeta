// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * SPDX-FileCopyrightText: © 2026 RockeyWereWolf <werewolfoffers@protonmail.com>
 * SPDX-FileCopyrightText: © 2011 Mojang AB
 *
 * C++ implementation of original game Perlin noise.
 *
 * Features specific "Bug Replication" regarding Y-gradient recalculation
 * timing and manual floor handling to ensure perfect terrain parity.
 *
 * References decompiled code from Retro-MCP.
 */

#pragma once
#include "../core/JavaRandom.hpp"
#include <vector>

class PerlinNoise {
private:
  int permutations[512];
  double xCoord, yCoord, zCoord;

public:
  PerlinNoise(JavaRandom &rand) {
    xCoord = rand.nextDouble() * 256.0;
    yCoord = rand.nextDouble() * 256.0;
    zCoord = rand.nextDouble() * 256.0;

    for (int i = 0; i < 256; ++i)
      permutations[i] = i;

    for (int i = 0; i < 256; ++i) {
      int j = rand.nextInt(256 - i) + i;
      int temp = permutations[i];
      permutations[i] = permutations[j];
      permutations[j] = temp;
      permutations[i + 256] = permutations[i];
    }
  }

  inline double lerp(double t, double a, double b) { return a + t * (b - a); }

  inline double grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
  }

  inline double grad2D(int hash, double x, double z) {
    int h = hash & 15;
    double u = (1 - ((h & 8) >> 3)) * x;
    double v = h < 4 ? 0.0 : (h == 12 || h == 14 ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
  }

  void populateNoiseArray(double *array, double xOffset, double yOffset,
                          double zOffset, int xSize, int ySize, int zSize,
                          double xScale, double yScale, double zScale,
                          double amp) {

    double invAmp = 1.0 / amp;
    int idx = 0;

    if (ySize == 1) {
      for (int x = 0; x < xSize; ++x) {
        double xVal = (xOffset + (double)x) * xScale + xCoord;
        int xInt = (int)xVal;
        if (xVal < (double)xInt)
          --xInt;
        int xHash = xInt & 255;
        xVal -= (double)xInt;
        double u = xVal * xVal * xVal * (xVal * (xVal * 6.0 - 15.0) + 10.0);

        for (int z = 0; z < zSize; ++z) {
          double zVal = (zOffset + (double)z) * zScale + zCoord;
          int zInt = (int)zVal;
          if (zVal < (double)zInt)
            --zInt;
          int zHash = zInt & 255;
          zVal -= (double)zInt;
          double w = zVal * zVal * zVal * (zVal * (zVal * 6.0 - 15.0) + 10.0);

          int pX = permutations[xHash] + 0;
          int pAA = permutations[pX] + zHash; // Hash(x, 0, z)

          int pB = permutations[xHash + 1] + 0;
          int pBA = permutations[pB] + zHash; // Hash(x+1, 0, z)

          double lerp1 = lerp(u, grad2D(permutations[pAA], xVal, zVal),
                              grad(permutations[pBA], xVal - 1.0, 0.0, zVal));

          double lerp2 =
              lerp(u, grad(permutations[pAA + 1], xVal, 0.0, zVal - 1.0),
                   grad(permutations[pBA + 1], xVal - 1.0, 0.0, zVal - 1.0));

          double val = lerp(w, lerp1, lerp2);
          array[idx++] += val * invAmp;
        }
      }
      return;
    }

    // STANDARD 3D CASE
    double lerp1, lerp2, lerp3, lerp4;
    int lastYInt = -1;

    for (int x = 0; x < xSize; ++x) {
      double xVal = (xOffset + (double)x) * xScale + xCoord;
      int xInt = (int)xVal;
      if (xVal < (double)xInt)
        --xInt;
      int xHash = xInt & 255;
      xVal -= (double)xInt;
      double u = xVal * xVal * xVal * (xVal * (xVal * 6.0 - 15.0) + 10.0);

      for (int z = 0; z < zSize; ++z) {
        double zVal = (zOffset + (double)z) * zScale + zCoord;
        int zInt = (int)zVal;
        if (zVal < (double)zInt)
          --zInt;
        int zHash = zInt & 255;
        zVal -= (double)zInt;
        double w = zVal * zVal * zVal * (zVal * (zVal * 6.0 - 15.0) + 10.0);

        for (int y = 0; y < ySize; ++y) {
          double yVal = (yOffset + (double)y) * yScale + yCoord;
          int yInt = (int)yVal;
          if (yVal < (double)yInt)
            --yInt;
          int yHash = yInt & 255;
          yVal -= (double)yInt;
          double v = yVal * yVal * yVal * (yVal * (yVal * 6.0 - 15.0) + 10.0);

          // BUG REPLICATION:
          // Only recalculate the X-interpolated gradients if we are at the
          // start of the column (y==0) OR if we have moved to a new integer Y
          // cell.
          if (y == 0 || yHash != lastYInt) {
            lastYInt = yHash;

            int pX = permutations[xHash] + yHash;
            int pAA = permutations[pX] + zHash;
            int pAB = permutations[pX + 1] + zHash;
            int pB = permutations[xHash + 1] + yHash;
            int pBA = permutations[pB] + zHash;
            int pBB = permutations[pB + 1] + zHash;

            lerp1 = lerp(u, grad(permutations[pAA], xVal, yVal, zVal),
                         grad(permutations[pBA], xVal - 1.0, yVal, zVal));

            lerp2 = lerp(u, grad(permutations[pAB], xVal, yVal - 1.0, zVal),
                         grad(permutations[pBB], xVal - 1.0, yVal - 1.0, zVal));

            lerp3 =
                lerp(u, grad(permutations[pAA + 1], xVal, yVal, zVal - 1.0),
                     grad(permutations[pBA + 1], xVal - 1.0, yVal, zVal - 1.0));

            lerp4 = lerp(
                u, grad(permutations[pAB + 1], xVal, yVal - 1.0, zVal - 1.0),
                grad(permutations[pBB + 1], xVal - 1.0, yVal - 1.0,
                     zVal - 1.0));
          }

          double interpY1 = lerp(v, lerp1, lerp2);
          double interpY2 = lerp(v, lerp3, lerp4);

          double val = lerp(w, interpY1, interpY2);
          array[idx++] += val * invAmp;
        }
      }
    }
  }
};

class NoiseOctaves {
private:
  std::vector<PerlinNoise> generators;
  int octaves;

public:
  NoiseOctaves(JavaRandom &rand, int octaves) : octaves(octaves) {
    for (int i = 0; i < octaves; ++i) {
      generators.push_back(PerlinNoise(rand));
    }
  }

  void generate(double *noiseArray, double x, double y, double z, int xSize,
                int ySize, int zSize, double xScale, double yScale,
                double zScale) {
    for (int i = 0; i < xSize * ySize * zSize; ++i)
      noiseArray[i] = 0.0;

    double amp = 1.0;
    for (int i = 0; i < octaves; ++i) {
      generators[i].populateNoiseArray(
          noiseArray, (double)x, (double)y, (double)z, xSize, ySize, zSize,
          xScale * amp, yScale * amp, zScale * amp, amp);
      amp /= 2.0;
    }
  }
};