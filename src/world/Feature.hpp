#pragma once
#include "../core/JavaRandom.hpp"
#include "Chunk.hpp"

class WorldGenerator {
public:
  virtual ~WorldGenerator() = default;

  virtual bool generate(class WorldInterface &world, JavaRandom &rand, int x,
                        int y, int z) = 0;
};

class WorldInterface {
public:
  virtual ~WorldInterface() = default;
  virtual int getBlockId(int x, int y, int z) = 0;
  virtual void setBlock(int x, int y, int z, int id) = 0;

  virtual int getBlockMeta(int x, int y, int z) { return 0; }
  virtual void setBlock(int x, int y, int z, int id, int meta) {
    setBlock(x, y, z, id);
  }
};
