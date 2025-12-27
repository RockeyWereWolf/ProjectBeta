#include "../core/JavaRandom.hpp"
#include <iomanip>
#include <iostream>

int main_testRandom() {
  JavaRandom rnd(0);
  // Brute Force Search
  for (int skip = 0; skip < 10; ++skip) {
    rnd.setSeed(0);

    // Skip 'skip' doubles
    for (int i = 0; i < skip; ++i)
      rnd.nextDouble();

    // Simulate Bedrock Check at Y=1
    // We need to simulate Y=127 down to 1
    // Each Y calls nextInt(5).

    // But wait, the column has 3 nextDoubles BEFORE y loop.
    // Let's assume standard 3 doubles + N skips.

    // Actually, let's try assuming 3 doubles ARE CALLED.
    // And we skip extra?
    // Or assume FEWER doubles called?

    // Scenario A: Standard 3 doubles
    double d1 = rnd.nextDouble();
    double d2 = rnd.nextDouble();
    double d3 = rnd.nextDouble();

    bool foundZeroAt1 = false;
    for (int y = 127; y >= 0; --y) {
      int r = rnd.nextInt(5);
      if (y == 1) {
        if (r == 0)
          foundZeroAt1 = true;
        // If r < 1 (0), it places Bedrock? No if (y <= r).
        // if (1 <= r). If r=0, False. Java Found Stone (Expected 1).
        // So Java had r=0.
        if (r == 0) {
          std::cout << "MATCH FOUND with skip=" << skip
                    << " (User had Stone at Y=1, we simulate r=0)" << std::endl;
        }
      }
    }
  }

  // Scenario B: Fewer doubles?
  for (int count = 0; count < 3; ++count) {
    rnd.setSeed(0);
    for (int i = 0; i < count; ++i)
      rnd.nextDouble(); // 0, 1, 2 calls

    for (int y = 127; y >= 1; --y) {
      int r = rnd.nextInt(5);
      if (y == 1 && r == 0) {
        std::cout << "MATCH FOUND with ONLY " << count << " doubles called."
                  << std::endl;
      }
    }
  }
  return 0;
}
