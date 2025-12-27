#pragma once
#include <cmath>
#include <vector>

class MathHelper {
private:
  static float SIN_TABLE[65536];
  static bool initialized;

public:
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
};

float MathHelper::SIN_TABLE[65536];
bool MathHelper::initialized = false;
