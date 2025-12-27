#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

// --- JavaRandom (Already Confirmed Working) ---
class JavaRandom {
private:
  uint64_t seed;
  static const uint64_t MULTIPLIER = 25214903917ULL;
  static const uint64_t ADDEND = 11ULL;
  static const uint64_t MASK = 281474976710655ULL;

  int32_t next(int bits) {
    seed = (seed * MULTIPLIER + ADDEND) & MASK;
    return (int32_t)((int64_t)seed >> (48 - bits));
  }

public:
  JavaRandom(int64_t s) { seed = ((uint64_t)s ^ MULTIPLIER) & MASK; }
  int32_t nextInt(int32_t bound) {
    if ((bound & -bound) == bound) {
      return (int32_t)((bound * (int64_t)next(31)) >> 31);
    }
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
};

// --- MineSimplex (To Test) ---
class MineSimplex {
private:
  int p[512];
  double xo, yo, zo;
  static constexpr double F2 = 0.5 * (1.7320508075688772 - 1.0);
  static constexpr double G2 = (3.0 - 1.7320508075688772) / 6.0;

  int fastFloor(double x) { return (int)std::floor(x); }
  double dot(const int *g, double x, double y) { return g[0] * x + g[1] * y; }

public:
  MineSimplex(JavaRandom &rand) {
    xo = rand.nextDouble() * 256.0;
    yo = rand.nextDouble() * 256.0;
    zo = rand.nextDouble() * 256.0;
    for (int i = 0; i < 512; ++i)
      p[i] = rand.nextInt(256);
  }

  void addNoise(std::vector<double> &arr, double x, double z, int xSize,
                int zSize, double scaleX, double scaleZ, double amp) {
    static const int grad3[12][3] = {{1, 1, 0},   {-1, 1, 0},  {1, -1, 0},
                                     {-1, -1, 0}, {1, 0, 1},   {-1, 0, 1},
                                     {1, 0, -1},  {-1, 0, -1}, {0, 1, 1},
                                     {0, -1, 1},  {0, 1, -1},  {0, -1, -1}};

    int index = 0;
    for (int i = 0; i < xSize; ++i) {
      double dx = (x + i) * scaleX + xo;
      for (int j = 0; j < zSize; ++j) {
        double dz = (z + j) * scaleZ + yo;

        double s = (dx + dz) * F2;
        int i1 = fastFloor(dx + s);
        int j1 = fastFloor(dz + s);
        double t = (i1 + j1) * G2;
        double X0 = i1 - t;
        double Y0 = j1 - t;
        double x0 = dx - X0;
        double y0 = dz - Y0;

        int i2, j2;
        if (x0 > y0) {
          i2 = 1;
          j2 = 0;
        } else {
          i2 = 0;
          j2 = 1;
        }

        double x1 = x0 - i2 + G2;
        double y1 = y0 - j2 + G2;
        double x2 = x0 - 1.0 + 2.0 * G2;
        double y2 = y0 - 1.0 + 2.0 * G2;

        int ii = i1 & 255;
        int jj = j1 & 255;
        int gi0 = p[ii + p[jj]] % 12;
        int gi1 = p[ii + i2 + p[jj + j2]] % 12;
        int gi2 = p[ii + 1 + p[jj + 1]] % 12;

        double t0 = 0.5 - x0 * x0 - y0 * y0;
        double n0 = 0;
        if (t0 >= 0) {
          t0 *= t0;
          n0 = t0 * t0 * dot(grad3[gi0], x0, y0);
        }

        double t1 = 0.5 - x1 * x1 - y1 * y1;
        double n1 = 0;
        if (t1 >= 0) {
          t1 *= t1;
          n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
        }

        double t2 = 0.5 - x2 * x2 - y2 * y2;
        double n2 = 0;
        if (t2 >= 0) {
          t2 *= t2;
          n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
        }

        arr[index++] += 70.0 * (n0 + n1 + n2) * amp;
      }
    }
  }
};

int main1() {
  int64_t worldSeed = -100;
  int64_t biomeSeed = worldSeed * 9871;

  std::cout << "--- C++ MATH CHECK ---" << std::endl;
  std::cout << "Biome Seed: " << biomeSeed << std::endl;
  std::cout << std::setprecision(17);

  JavaRandom rand(biomeSeed);
  MineSimplex simplex(rand); // This consumes init RNG

  // Test a single point generation
  // Emulate Chunk 0, 0 generation logic
  // x=0, z=0, scale=0.025, amp=0.25
  std::vector<double> result(1);
  result[0] = 0.0;

  simplex.addNoise(result, 0.0, 0.0, 1, 1, 0.025, 0.025, 0.25);

  std::cout << "Noise Value at (0,0): " << result[0] << std::endl;

  return 0;
}