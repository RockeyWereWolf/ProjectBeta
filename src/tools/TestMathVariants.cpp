#include <cstdint>
#include <iomanip>
#include <iostream>

int main() {
  // This is the RAW NUMERATOR that matches Java (Call 1)
  int64_t numerator = 6583972509698697LL;
  int64_t denominator_long = 1LL << 53;
  double denominator_dbl = 9007199254740992.0;

  std::cout << "Target (Java): 0.73096778737665700" << std::endl;
  std::cout << std::fixed << std::setprecision(17);

  // Variant 1: Standard Division (What we have now)
  double v1 = numerator / (double)denominator_long;
  std::cout << "Variant 1 (Std): " << v1
            << (v1 == 0.73096778737665700 ? " [MATCH]" : " [FAIL]")
            << std::endl;

  // Variant 2: Multiplication by Inverse
  // Sometimes multiplying by (1/denom) rounds differently than dividing by
  // denom
  double v2 = (double)numerator * (1.0 / denominator_dbl);
  std::cout << "Variant 2 (Mul): " << v2
            << (v2 == 0.73096778737665700 ? " [MATCH]" : " [FAIL]")
            << std::endl;

  // Variant 3: Long Double (Extended Precision)
  // We force the math to happen in 80-bit/128-bit space, then cast down.
  double v3 = (double)((long double)numerator / (long double)denominator_long);
  std::cout << "Variant 3 (LDB): " << v3
            << (v3 == 0.73096778737665700 ? " [MATCH]" : " [FAIL]")
            << std::endl;

  return 0;
}