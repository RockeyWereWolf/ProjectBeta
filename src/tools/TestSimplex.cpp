#include "../core/JavaRandom.hpp"
#include "../world/SimplexNoise.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

int main() {
  int64_t seed = -100L;
  JavaRandom random(seed);
  SimplexNoise simplex(random);

  std::vector<double> noise(1);

  // Test at x=0.5, y=0.5
  simplex.func_4157_a(noise.data(), 0.5, 0.5, 1, 1, 1.0, 1.0, 1.0);

  std::cout << "=== C++ SIMPLEX CHECK (0.5, 0.5) ===" << std::endl;
  std::cout << std::fixed << std::setprecision(20);
  std::cout << "Value: " << noise[0] << std::endl;

  return 0;
}