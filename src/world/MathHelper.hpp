// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * SPDX-FileCopyrightText: © 2026 RockeyWereWolf <werewolfoffers@protonmail.com>
 * SPDX-FileCopyrightText: © 2011 Mojang AB
 *
 * C++ implementation of original game internal MathHelper class.
 *
 * Crucial for achieving bit-perfect parity in noise and entity math.
 *
 * References decompiled code from Retro-MCP.
 */

#pragma once
#include <cmath>

class MathHelper {
private:
  static bool initialized;

public:
  static float SIN_TABLE[65536];

  static void Init() {
    if (initialized)
      return;
    for (int i = 0; i < 65536; ++i) {
      SIN_TABLE[i] =
          (float)std::sin((double)i * 3.141592653589793 * 2.0 / 65536.0);
    }
    initialized = true;
  }

  static float sin(float value) {
    if (!initialized)
      Init();
    return SIN_TABLE[(int)(value * 10430.378f) & 65535];
  }

  static float cos(float value) {
    if (!initialized)
      Init();
    return SIN_TABLE[(int)(value * 10430.378f + 16384.0f) & 65535];
  }

  static int floor(double value) {
    int i = (int)value;
    return value < (double)i ? i - 1 : i;
  }

  static double lerp(double t, double a, double b) { return a + t * (b - a); }

  template <typename T> static constexpr T MC_PI() {
    return T(3.14159265358979323846L);
  }
};

float MathHelper::SIN_TABLE[65536];
bool MathHelper::initialized = false;
