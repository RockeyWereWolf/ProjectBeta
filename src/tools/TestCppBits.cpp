#include "../core/JavaRandom.hpp"
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>

// Helper to convert double to raw bits safely
uint64_t doubleToBits(double d) {
  uint64_t bits;
  std::memcpy(&bits, &d, sizeof(bits));
  return bits;
}

int main() {
  JavaRandom rng(0LL);

  std::cout << "=== C++ RAW BITS CHECK ===" << std::endl;

  double val1 = rng.nextDouble();
  uint64_t bits1 = doubleToBits(val1);

  std::cout << std::fixed << std::setprecision(20);
  std::cout << "Call 1 Value: " << val1 << std::endl;
  // Print hex with 0-padding, uppercase
  std::cout << "Call 1 Bits:  0x" << std::hex << std::uppercase << std::setw(16)
            << std::setfill('0') << bits1 << std::endl;

  return 0;
}