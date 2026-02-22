// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * SPDX-FileCopyrightText: © 2026 RockeyWereWolf <werewolfoffers@protonmail.com>
 * SPDX-FileCopyrightText: © 2011 Mojang AB (Original Biome Logic)
 *
 * C++ implementation of original game biome data generation.
 * Handles temperature/rainfall noise maps and biome lookup table.
 *
 * References decompiled code from Retro-MCP.
 */

#pragma once
#include "../core/JavaRandom.hpp"
#include "SimplexNoise.hpp"
#include <vector>

enum class BiomeID {
  Rainforest = 0,
  Swampland = 1,
  SeasonalForest = 2,
  Forest = 3,
  Savanna = 4,
  Shrubland = 5,
  Taiga = 6,
  Desert = 7,
  Plains = 8,
  IceDesert = 9,
  Tundra = 10,
  Hell = 11,
  Sky = 12
};

struct BiomeProperties {
  BiomeID id;
  uint8_t topBlock;
  uint8_t fillerBlock;
  BiomeProperties() : id(BiomeID::Plains), topBlock(2), fillerBlock(3) {}
  BiomeProperties(BiomeID i, uint8_t t, uint8_t f)
      : id(i), topBlock(t), fillerBlock(f) {}
};

class SimplexOctaves {
private:
  std::vector<SimplexNoise> generators;
  int octaves;

public:
  SimplexOctaves(JavaRandom &rand, int octaves) : octaves(octaves) {
    for (int i = 0; i < octaves; ++i)
      generators.push_back(SimplexNoise(rand));
  }

  void generateForBiomes(double *data, double x, double z, int sizeX, int sizeZ,
                         double scaleX, double scaleZ, double lacunarity) {
    for (int i = 0; i < sizeX * sizeZ; ++i)
      data[i] = 0.0;

    double freq = 1.0;
    double amp = 1.0;

    scaleX /= 1.5;
    scaleZ /= 1.5;

    for (int i = 0; i < octaves; ++i) {
      generators[i].func_4157_a(data, x, z, sizeX, sizeZ, scaleX * freq,
                                scaleZ * freq, 0.55 / amp);
      freq *= lacunarity;
      amp *= 0.5;
    }
  }
};

class BiomeManager {
private:
  SimplexOctaves *tempNoise;
  SimplexOctaves *rainNoise;
  SimplexOctaves *detailNoise;

  double *tempArray;
  double *rainArray;
  double *detailArray;

public:
  BiomeManager(int64_t seed) {

    JavaRandom tempRand(seed * 9871L);
    tempNoise = new SimplexOctaves(tempRand, 4);

    JavaRandom rainRand(seed * 39811L);
    rainNoise = new SimplexOctaves(rainRand, 4);

    JavaRandom detailRand(seed * 543321L);
    detailNoise = new SimplexOctaves(detailRand, 2);

    tempArray = new double[1];
    rainArray = new double[1];
    detailArray = new double[1];
  }

  ~BiomeManager() {
    delete tempNoise;
    delete rainNoise;
    delete detailNoise;
    delete[] tempArray;
    delete[] rainArray;
    delete[] detailArray;
  }

  void getBiomeDataForRegion(double *tempOutput, double *rainOutput, int startX,
                             int startZ, int sizeX, int sizeZ) {
    if (detailArray == nullptr || (sizeX * sizeZ) > 1) {
      delete[] detailArray;
      detailArray = new double[sizeX * sizeZ];
    }

    tempNoise->generateForBiomes(tempOutput, (double)startX, (double)startZ,
                                 sizeX, sizeZ, 0.025, 0.025, 0.25);
    rainNoise->generateForBiomes(rainOutput, (double)startX, (double)startZ,
                                 sizeX, sizeZ, 0.05, 0.05, 1.0 / 3.0);
    detailNoise->generateForBiomes(detailArray, (double)startX, (double)startZ,
                                   sizeX, sizeZ, 0.25, 0.25,
                                   0.5882352941176471);

    int idx = 0;
    for (int i = 0; i < sizeX; ++i) {
      for (int j = 0; j < sizeZ; ++j) {
        double detail = detailArray[idx] * 1.1 + 0.5;
        double epsilonTemp = 0.01;
        double invEpsilonTemp = 1.0 - epsilonTemp;

        double t = (tempOutput[idx] * 0.15 + 0.7) * invEpsilonTemp +
                   detail * epsilonTemp;
        t = 1.0 - (1.0 - t) * (1.0 - t);
        if (t < 0.0)
          t = 0.0;
        if (t > 1.0)
          t = 1.0;
        tempOutput[idx] = t;

        double epsilonRain = 0.002;
        double invEpsilonRain = 1.0 - epsilonRain;

        double r = (rainOutput[idx] * 0.15 + 0.5) * invEpsilonRain +
                   detail * epsilonRain;
        if (r < 0.0)
          r = 0.0;
        if (r > 1.0)
          r = 1.0;
        rainOutput[idx] = r;
        idx++;
      }
    }
  }

  BiomeProperties getBiomeProps(int x, int z) {
    BiomeID id = getBiome(x, z);
    uint8_t top = 2;
    uint8_t fill = 3;
    switch (id) {
    case BiomeID::Desert:
    case BiomeID::IceDesert:
      top = 12;
      fill = 12;
      break;
    default:
      break;
    }
    return BiomeProperties(id, top, fill);
  }

  BiomeID getBiome(int globeX, int globeZ) {
    double temp = getTemp(globeX, globeZ);
    double rain = getRain(globeX, globeZ);
    return getBiomeFromValues(temp, rain);
  }

  double getTemp(int x, int z) {
    double t, r;
    getTempRain(x, z, t, r);
    return t;
  }
  double getRain(int x, int z) {
    double t, r;
    getTempRain(x, z, t, r);
    return r;
  }

  void getTempRain(int x, int z, double &outTemp, double &outRain) {
    tempNoise->generateForBiomes(tempArray, x, z, 1, 1, 0.025, 0.025, 0.25);
    rainNoise->generateForBiomes(rainArray, x, z, 1, 1, 0.05, 0.05, 1.0 / 3.0);
    detailNoise->generateForBiomes(detailArray, x, z, 1, 1, 0.25, 0.25,
                                   0.5882352941176471);

    double detail = detailArray[0] * 1.1 + 0.5;
    double epsilonTemp = 0.01;
    double invEpsilonTemp = 1.0 - epsilonTemp;

    double t =
        (tempArray[0] * 0.15 + 0.7) * invEpsilonTemp + detail * epsilonTemp;
    t = 1.0 - (1.0 - t) * (1.0 - t);
    if (t < 0)
      t = 0;
    if (t > 1)
      t = 1;

    double epsilonRain = 0.002;
    double invEpsilonRain = 1.0 - epsilonRain;

    double r =
        (rainArray[0] * 0.15 + 0.5) * invEpsilonRain + detail * epsilonRain;
    if (r < 0)
      r = 0;
    if (r > 1)
      r = 1;

    outTemp = t;
    outRain = r;
  }

  BiomeID getBiomeFromValues(double temp, double rain) {
    int iTemp = (int)(temp * 63.0);
    int iRain = (int)(rain * 63.0);

    double t = (double)iTemp / 63.0;
    double r = (double)iRain / 63.0;

    r *= t;

    if (t < 0.1)
      return BiomeID::Tundra;
    if (r < 0.2) {
      if (t < 0.5)
        return BiomeID::Tundra;
      if (t < 0.95)
        return BiomeID::Savanna;
      return BiomeID::Desert;
    }
    if (r > 0.5 && t < 0.7)
      return BiomeID::Swampland;
    if (t < 0.5)
      return BiomeID::Taiga;
    if (t < 0.97) {
      if (r < 0.35)
        return BiomeID::Shrubland;
      return BiomeID::Forest;
    }
    if (r < 0.45)
      return BiomeID::Plains;
    if (r < 0.9)
      return BiomeID::SeasonalForest;
    return BiomeID::Rainforest;
  }

  int getTreesPerChunk(BiomeID biome) {
    switch (biome) {
    case BiomeID::Rainforest:
      return 10;
    case BiomeID::Swampland:
      return 2;
    case BiomeID::SeasonalForest:
      return 4;
    case BiomeID::Forest:
      return 10;
    case BiomeID::Savanna:
      return 0;
    case BiomeID::Shrubland:
      return 0;
    case BiomeID::Taiga:
      return 10;
    case BiomeID::Desert:
      return 0;
    case BiomeID::Plains:
      return 0;
    case BiomeID::IceDesert:
      return 0;
    case BiomeID::Tundra:
      return 0;
    default:
      return 0;
    }
  }
};