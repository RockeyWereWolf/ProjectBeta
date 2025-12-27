#include <cstdint>
#include <iomanip>
#include <iostream>

int main2() {
  // Inputs
  int64_t seedIn = -987100;
  int64_t multIn = 25214903917LL;
  int64_t maskIn = 281474976710655LL;

  // Cast to Unsigned 64-bit to prevent sign extension/padding issues
  uint64_t uSeed = (uint64_t)seedIn;
  uint64_t uMult = (uint64_t)multIn;
  uint64_t uMask = (uint64_t)maskIn;

  // Operation
  uint64_t result = (uSeed ^ uMult) & uMask;

  std::cout << std::hex << std::uppercase;
  std::cout << "--- 64-BIT MATH DEBUG ---" << std::endl;
  std::cout << "Seed (Hex): " << uSeed << std::endl;
  std::cout << "Mult (Hex): " << uMult << std::endl;
  std::cout << "XOR  (Hex): " << (uSeed ^ uMult) << std::endl;
  std::cout << "Mask (Hex): " << uMask << std::endl;
  std::cout << "Res  (Hex): " << result << std::endl;

  std::cout << std::dec;
  std::cout << "Res  (Dec): " << result << std::endl;

  // Expected: 0xFFFA2E1CC449 (281464303633481)
  if (result == 281464303633481ULL) {
    std::cout << "PASS: Unsigned Math works." << std::endl;
  } else {
    std::cout << "FAIL: Mismatch." << std::endl;
  }

  return 0;
}