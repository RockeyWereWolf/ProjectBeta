#pragma once
#include "../core/JavaRandom.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

static int simplexCount = 0;

class SimplexNoise {
private:
  static constexpr double F2 = 0.5 * (1.7320508075688772 - 1.0);
  static constexpr double G2 = (3.0 - 1.7320508075688772) / 6.0;
  static const int grad3[12][3];
  int p[512];
  double xo, yo,
      zo; // these are public: field_4313_a, field_4312_b, field_4318_c

public:
  SimplexNoise(JavaRandom &rand) {

    int id = simplexCount++;
    xo = rand.nextDouble() * 256.0;
    yo = rand.nextDouble() * 256.0;
    zo = rand.nextDouble() * 256.0;

    for (int i = 0; i < 256; i++)
      p[i] = i;
    for (int i = 0; i < 256; i++) {
      int j = rand.nextInt(256 - i) + i;
      int temp = p[i];
      p[i] = p[j];
      p[j] = temp;
      p[i + 256] = p[i];
    }
  }

  // Java's wrap/fastFloor
  inline static int fastFloor(double x) {
    return x > 0.0 ? (int)x : (int)x - 1;
  }
  // dot product func_4114_a
  inline static double dot(const int *g, double x, double y) {
    return (double)g[0] * x + (double)g[1] * y;
  }

  //  func_4115_a
  void func_4157_a(double *noiseArray, double xPos, double zPos, int xSize,
                   int zSize, double scaleX, double scaleZ, double scaleAmp) {
    int index = 0;
    for (int i = 0; i < xSize; ++i) {
      double x = (xPos + (double)i) * scaleX + xo;
      for (int j = 0; j < zSize; ++j) {
        double z = (zPos + (double)j) * scaleZ + yo;

        double s = (x + z) * F2;
        int i1 = fastFloor(x + s);
        int j1 = fastFloor(z + s);
        double t = (double)(i1 + j1) * G2;
        double X0 = (double)i1 - t;
        double Y0 = (double)j1 - t;
        double x0 = x - X0;
        double y0 = z - Y0;

        int i2, j2;
        if (x0 > y0) {
          i2 = 1;
          j2 = 0;
        } else {
          i2 = 0;
          j2 = 1;
        }

        double x1 = x0 - (double)i2 + G2;
        double y1 = y0 - (double)j2 + G2;
        double x2 = x0 - 1.0 + 2.0 * G2;
        double y2 = y0 - 1.0 + 2.0 * G2;

        int ii = i1 & 255;
        int jj = j1 & 255;
        int gi0 = p[ii + p[jj]] % 12;
        int gi1 = p[ii + i2 + p[jj + j2]] % 12;
        int gi2 = p[ii + 1 + p[jj + 1]] % 12;

        double t0 = 0.5 - x0 * x0 - y0 * y0;
        double n0;
        if (t0 < 0.0) {
          n0 = 0.0;
        } else {
          t0 *= t0;
          n0 = t0 * t0 * dot(grad3[gi0], x0, y0);
        }

        double t1 = 0.5 - x1 * x1 - y1 * y1;
        double n1;
        if (t1 < 0.0) {
          n1 = 0.0;
        } else {
          t1 *= t1;
          n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
        }

        double t2 = 0.5 - x2 * x2 - y2 * y2;
        double n2;
        if (t2 < 0.0) {
          n2 = 0.0;
        } else {
          t2 *= t2;
          n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
        }

        noiseArray[index++] += 70.0 * (n0 + n1 + n2) * scaleAmp;
      }
    }
  }
};

inline const int SimplexNoise::grad3[12][3] = {
    {1, 1, 0},  {-1, 1, 0},  {1, -1, 0}, {-1, -1, 0}, {1, 0, 1},  {-1, 0, 1},
    {1, 0, -1}, {-1, 0, -1}, {0, 1, 1},  {0, -1, 1},  {0, 1, -1}, {0, -1, -1}};