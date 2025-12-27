
#include "../world/WorldGen.hpp"
#include <iostream>

int main() {
  std::cout << "Compiling WorldGen..." << std::endl;
  // Just instantiate the classes to trigger compilation of headers
  WorldGen w(12345);
  WorldGenTrees t;
  WorldGenForest f;
  WorldGenTaiga1 ta1;
  WorldGenTaiga2 ta2;
  WorldGenBigTree bt;
  WorldGenClay c(32);

  // w.setBlockAndMetadata(...) is not exposed on WorldGen directly.
  // It is used internally by populate() via SimpleWorldAdapter.

  // Check polymorphism
  WorldGenerator *gen = &f;
  gen = &ta2;

  std::cout << "Compilation Successful!" << std::endl;
  return 0;
}
