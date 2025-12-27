#include <iomanip>
#include <iostream>
#include <vector>

// Include the REAL WorldGen class header
#include "../world/WorldGen.hpp"

int main() {
  // 1. Create an instance of the WorldGen class, just like in main.cpp
  WorldGen worldGen(-100L);

  // 2. Create a dummy chunk to pass to the function
  Chunk chunk(0, 0);
  // Fill it with stone to match the old test's assumption
  for (int x = 0; x < 16; ++x) {
    for (int z = 0; z < 16; ++z) {
      for (int y = 0; y < 128; ++y) {
        chunk.setBlock(x, y, z, 1);
      }
    }
  }

  // 3. Call the real replaceBlocksForBiome method
  // Note: We need to make this method public temporarily to call it.
  // Go to WorldGen.hpp and move `replaceBlocksForBiome` to the `public:`
  // section.
  worldGen.replaceBlocksForBiome(chunk);

  // The output will come from the debug prints that are already inside
  // your `replaceBlocksForBiome` function.

  return 0;
}