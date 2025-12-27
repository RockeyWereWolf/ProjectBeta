#include <stdint.h>

void setSeed(uint64_t *seed, uint64_t value) {
  *seed = (value ^ 0x5deece66d) & ((1ULL << 48) - 1);
}

int next(uint64_t *seed, const int bits) {
  *seed = (*seed * 0x5deece66d + 0xb) & ((1ULL << 48) - 1);
  return (int)((int64_t)*seed >> (48 - bits));
}

double nextDouble(uint64_t *seed) {
  uint64_t x = (uint64_t)next(seed, 26);
  x <<= 27;
  x += next(seed, 27);
  return (int64_t)x / (double)(1ULL << 53);
}