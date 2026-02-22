// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * SPDX-FileCopyrightText: © 2026 RockeyWereWolf <werewolfoffers@protonmail.com>
 *
 * java.util.Random C++ port for a functional parity
 */

#pragma once
#include <cstdint>

class JavaRandom {
private:
  uint64_t seed;
  static const uint64_t MULTIPLIER = 25214903917ULL;
  static const uint64_t ADDEND = 11ULL;
  static const uint64_t MASK = 281474976710655ULL;

  int32_t next(int bits) {
    seed = (seed * MULTIPLIER + ADDEND) & MASK;
    return (int32_t)(seed >> (48 - bits));
  }

public:
  JavaRandom(int64_t s) { seed = ((uint64_t)s ^ MULTIPLIER) & MASK; }
  void setSeed(int64_t s) { seed = ((uint64_t)s ^ MULTIPLIER) & MASK; }
  int32_t nextInt() { return next(32); }
  int32_t nextInt(int32_t bound) {
    if (bound <= 0)
      return 0;
    if ((bound & -bound) == bound)
      return (int32_t)((bound * (int64_t)next(31)) >> 31);
    int32_t bits, val;
    do {
      bits = next(31);
      val = bits % bound;
    } while (bits - val + (bound - 1) < 0);
    return val;
  }
  double nextDouble() {
    return (((int64_t)next(26) << 27) + next(27)) / (double)(1LL << 53);
  }
  float nextFloat() { return next(24) / ((float)(1 << 24)); }
  int64_t nextLong() { return ((int64_t)next(32) << 32) + next(32); }

  int64_t getNextDoubleRawNumerator() {
    return ((int64_t)next(26) << 27) + next(27);
  }
};