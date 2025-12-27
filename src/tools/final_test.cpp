#include <cstdint>
#include <iomanip>
#include <iostream>

// Tell the C++ compiler that these functions are from a C library.
extern "C" {
void setSeed(uint64_t *seed, uint64_t value);
double nextDouble(uint64_t *seed);
}

int main() {
  uint64_t rng_seed;
  setSeed(&rng_seed, 0ULL);

  std::cout << "=== Final Test: True C Library Architecture ===\n" << std::endl;

  double val1 = nextDouble(&rng_seed);
  std::cout << "--- Call 1 ---" << std::endl;
  std::cout << std::fixed << std::setprecision(17);
  std::cout << "nextDouble():  " << val1 << std::endl;

  double val2 = nextDouble(&rng_seed);
  std::cout << "\n--- Call 2 ---" << std::endl;
  std::cout << "nextDouble():  " << val2 << std::endl;

  return 0;
}