#pragma once
#include <cstdint>
#include <vector>

// Compact Block representation
// Beta 1.7.3 Chunk Size: 16 width (X), 128 height (Y), 16 depth (Z)
constexpr int CHUNK_WIDTH = 16;
constexpr int CHUNK_HEIGHT = 128;
constexpr int CHUNK_DEPTH = 16;

namespace BlockID {
constexpr uint8_t Air = 0;
constexpr uint8_t Stone = 1;
constexpr uint8_t Grass = 2;
constexpr uint8_t Dirt = 3;
constexpr uint8_t Cobblestone = 4;
constexpr uint8_t Wood = 5;
constexpr uint8_t Sapling = 6;
constexpr uint8_t Bedrock = 7;
constexpr uint8_t Water = 8;
constexpr uint8_t StationaryWater = 9;
constexpr uint8_t Lava = 10;
constexpr uint8_t StationaryLava = 11;
constexpr uint8_t Sand = 12;
constexpr uint8_t Gravel = 13;
constexpr uint8_t GoldOre = 14;
constexpr uint8_t IronOre = 15;
constexpr uint8_t CoalOre = 16;
constexpr uint8_t Log = 17;
constexpr uint8_t Leaves = 18;
constexpr uint8_t Sponge = 19;
constexpr uint8_t Glass = 20;
constexpr uint8_t LapisOre = 21;
constexpr uint8_t LapisBlock = 22;
constexpr uint8_t Dispenser = 23;
constexpr uint8_t Sandstone = 24;
constexpr uint8_t NoteBlock = 25;
constexpr uint8_t Bed = 26;
constexpr uint8_t PoweredRail = 27;
constexpr uint8_t DetectorRail = 28;
constexpr uint8_t StickyPiston = 29;
constexpr uint8_t Web = 30;
constexpr uint8_t TallGrass = 31;
constexpr uint8_t DeadBush = 32;
constexpr uint8_t Piston = 33;
constexpr uint8_t PistonHead = 34;
constexpr uint8_t Wool = 35;
constexpr uint8_t PistonMoving = 36;
constexpr uint8_t YellowFlower = 37;
constexpr uint8_t RedFlower = 38;
constexpr uint8_t BrownMushroom = 39;
constexpr uint8_t RedMushroom = 40;
constexpr uint8_t GoldBlock = 41;
constexpr uint8_t IronBlock = 42;
constexpr uint8_t DoubleSlab = 43;
constexpr uint8_t Slab = 44;
constexpr uint8_t Brick = 45;
constexpr uint8_t TNT = 46;
constexpr uint8_t Bookshelf = 47;
constexpr uint8_t MossyCobblestone = 48;
constexpr uint8_t Obsidian = 49;
constexpr uint8_t Torch = 50;
constexpr uint8_t Fire = 51;
constexpr uint8_t MobSpawner = 52;
constexpr uint8_t WoodenStairs = 53;
constexpr uint8_t Chest = 54;
constexpr uint8_t RedstoneWire = 55;
constexpr uint8_t DiamondOre = 56;
constexpr uint8_t DiamondBlock = 57;
constexpr uint8_t CraftingTable = 58;
constexpr uint8_t Crops = 59;
constexpr uint8_t Farmland = 60;
constexpr uint8_t Furnace = 61;
constexpr uint8_t BurningFurnace = 62;
constexpr uint8_t SignPost = 63;
constexpr uint8_t WoodenDoor = 64;
constexpr uint8_t Ladder = 65;
constexpr uint8_t Rails = 66;
constexpr uint8_t CobblestoneStairs = 67;
constexpr uint8_t WallSign = 68;
constexpr uint8_t Lever = 69;
constexpr uint8_t StonePressurePlate = 70;
constexpr uint8_t IronDoor = 71;
constexpr uint8_t WoodenPressurePlate = 72;
constexpr uint8_t RedstoneOre = 73;
constexpr uint8_t GlowingRedstoneOre = 74;
constexpr uint8_t RedstoneTorchOff = 75;
constexpr uint8_t RedstoneTorchOn = 76;
constexpr uint8_t StoneButton = 77;
constexpr uint8_t SnowLayer = 78;
constexpr uint8_t Ice = 79;
constexpr uint8_t SnowBlock = 80;
constexpr uint8_t Cactus = 81;
constexpr uint8_t Clay = 82;
constexpr uint8_t Reed = 83;
constexpr uint8_t Jukebox = 84;
constexpr uint8_t Fence = 85;
constexpr uint8_t Pumpkin = 86;
constexpr uint8_t Netherrack = 87;
constexpr uint8_t SoulSand = 88;
constexpr uint8_t Glowstone = 89;
constexpr uint8_t Portal = 90;
constexpr uint8_t JackOLantern = 91;
constexpr uint8_t Cake = 92;
constexpr uint8_t RepeaterOff = 93;
constexpr uint8_t RepeaterOn = 94;
constexpr uint8_t LockedChest = 95;
constexpr uint8_t Trapdoor = 96;
} // namespace BlockID

struct Vertex {
  float x, y, z;
  float u, v;
  float r, g, b;
};

class Chunk {
public:
  uint8_t blocks[CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH];
  // Metadata (nibbles) would go here later (metadata[size/2])

  int chunkX, chunkZ;

  Chunk(int x, int z) : chunkX(x), chunkZ(z) {

    for (int i = 0; i < CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH; i++)
      blocks[i] = 0;
  }

  inline int getIndex(int x, int y, int z) const {
    return (x * 16 + z) * 128 + y;
  }

  uint8_t getBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_WIDTH || z < 0 || z >= CHUNK_DEPTH || y < 0 ||
        y >= CHUNK_HEIGHT)
      return 0;
    return blocks[getIndex(x, y, z)];
  }

  void setBlock(int x, int y, int z, uint8_t id) {
    if (x >= 0 && x < CHUNK_WIDTH && z >= 0 && z < CHUNK_DEPTH && y >= 0 &&
        y < CHUNK_HEIGHT) {
      blocks[getIndex(x, y, z)] = id;
    }
  }

  // Naive Meshing
  void generateMesh(std::vector<Vertex> &vertices, const Chunk *nX = nullptr,
                    const Chunk *pX = nullptr, const Chunk *nZ = nullptr,
                    const Chunk *pZ = nullptr) {
    vertices.clear();

    auto getRelBlock = [&](int bx, int by, int bz) -> uint8_t {
      if (by < 0 || by >= 128)
        return (uint8_t)0;
      if (bx < 0)
        return nX ? nX->getBlock(15, by, bz) : (uint8_t)0;
      if (bx >= 16)
        return pX ? pX->getBlock(0, by, bz) : (uint8_t)0;
      if (bz < 0)
        return nZ ? nZ->getBlock(bx, by, 15) : (uint8_t)0;
      if (bz >= 16)
        return pZ ? pZ->getBlock(bx, by, 0) : (uint8_t)0;
      return getBlock(bx, by, bz);
    };
    for (int x = 0; x < CHUNK_WIDTH; x++) {
      for (int z = 0; z < CHUNK_DEPTH; z++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
          uint8_t id = getBlock(x, y, z);
          if (id == 0)
            continue;

          float worldX = (float)(chunkX * 16 + x);
          float worldY = (float)y;
          float worldZ = (float)(chunkZ * 16 + z);

          // Determine Block Color
          float r = 1.0f, g = 1.0f, b = 1.0f;
          if (id == 1) { // Stone (Grey)
            r = 0.6f;
            g = 0.6f;
            b = 0.6f;
          } else if (id == 2) { // Grass (Green)
            r = 0.2f;
            g = 0.8f;
            b = 0.2f;
          } else if (id == 3) { // Dirt (Brown)
            r = 0.5f;
            g = 0.35f;
            b = 0.1f;
          } else if (id == 7) { // Bedrock (Dark Grey)
            r = 0.2f;
            g = 0.2f;
            b = 0.2f;
          } else if (id == 8 || id == 9) { // Water (Blue)
            r = 0.2f;
            g = 0.4f;
            b = 0.9f;
          } else if (id == 10 || id == 11) { // Lava (Orange)
            r = 1.0f;
            g = 0.4f;
            b = 0.0f;
          } else if (id == 12) { // Sand (Pale Yellow)
            r = 0.94f;
            g = 0.9f;
            b = 0.6f;
          } else if (id == 13) { // Gravel (Grey)
            r = 0.6f;
            g = 0.6f;
            b = 0.6f;
          } else if (id == 24) { // Sandstone (Yellowish Rock)
            r = 0.8f;
            g = 0.8f;
            b = 0.6f;
          } else if (id == BlockID::CoalOre) {
            r = 0.4f;
            g = 0.4f;
            b = 0.4f; // Dark Stone
          } else if (id == BlockID::IronOre) {
            r = 0.8f;
            g = 0.7f;
            b = 0.6f; // Brownish Stone
          } else if (id == BlockID::GoldOre) {
            r = 1.0f;
            g = 0.9f;
            b = 0.2f; // Gold
          } else if (id == BlockID::DiamondOre) {
            r = 0.3f;
            g = 0.9f;
            b = 0.8f; // Cyan
          } else if (id == BlockID::RedstoneOre ||
                     id == BlockID::GlowingRedstoneOre) {
            r = 0.9f;
            g = 0.1f;
            b = 0.1f; // Red
          } else if (id == BlockID::LapisOre) {
            r = 0.1f;
            g = 0.2f;
            b = 0.8f; // Blue
          } else if (id == BlockID::Bedrock) {

            r = 0.2f;
            g = 0.2f;
            b = 0.2f;
          } else if (id == BlockID::Log) {
            r = 0.4f;
            g = 0.3f;
            b = 0.1f; // Dark Brown
          } else if (id == BlockID::Leaves) {
            r = 0.2f;
            g = 0.6f;
            b = 0.1f; // Green
          }

          // Naive Culling

          // TOP (+Y)
          if (y == CHUNK_HEIGHT - 1 || getBlock(x, y + 1, z) == 0) {
            addQuad(vertices, worldX, worldY + 1, worldZ, 0, 1, 0, 1.0f, r, g,
                    b);
          }
          // BOTTOM (-Y)
          if (y == 0 || getBlock(x, y - 1, z) == 0) {
            addQuad(vertices, worldX, worldY, worldZ, 0, 1, 0, 0.5f, r, g, b);
          }
          // NORTH (-Z)
          if (getRelBlock(x, y, z - 1) == 0) {
            addQuad(vertices, worldX, worldY, worldZ, 0, 0, -1, 0.8f, r, g, b);
          }
          // SOUTH (+Z)
          if (getRelBlock(x, y, z + 1) == 0) {
            addQuad(vertices, worldX, worldY, worldZ + 1, 0, 0, 1, 0.8f, r, g,
                    b);
          }
          // WEST (-X)
          if (getRelBlock(x - 1, y, z) == 0) {
            addQuad(vertices, worldX, worldY, worldZ, -1, 0, 0, 0.6f, r, g, b);
          }
          // EAST (+X)
          if (getRelBlock(x + 1, y, z) == 0) {
            addQuad(vertices, worldX + 1, worldY, worldZ, 1, 0, 0, 0.6f, r, g,
                    b);
          }
        }
      }
    }
  }

private:
  void addQuad(std::vector<Vertex> &verts, float x, float y, float z, int axisX,
               int axisY, int axisZ, float shade, float colorR, float colorG,
               float colorB) {

    float p1x, p1y, p1z, p2x, p2y, p2z, p3x, p3y, p3z, p4x, p4y, p4z;

    if (axisY != 0) {
      p1x = x;
      p1y = y;
      p1z = z;
      p2x = x + 1;
      p2y = y;
      p2z = z;
      p3x = x + 1;
      p3y = y;
      p3z = z + 1;
      p4x = x;
      p4y = y;
      p4z = z + 1;
    } else if (axisZ != 0) {
      p1x = x;
      p1y = y;
      p1z = z;
      p2x = x + 1;
      p2y = y;
      p2z = z;
      p3x = x + 1;
      p3y = y + 1;
      p3z = z;
      p4x = x;
      p4y = y + 1;
      p4z = z;
    } else {
      p1x = x;
      p1y = y;
      p1z = z;
      p2x = x;
      p2y = y;
      p2z = z + 1;
      p3x = x;
      p3y = y + 1;
      p3z = z + 1;
      p4x = x;
      p4y = y + 1;
      p4z = z;
    }

    float finalR = colorR * shade;
    float finalG = colorG * shade;
    float finalB = colorB * shade;

    verts.push_back({p1x, p1y, p1z, 0, 0, finalR, finalG, finalB});
    verts.push_back({p2x, p2y, p2z, 1, 0, finalR, finalG, finalB});
    verts.push_back({p3x, p3y, p3z, 1, 1, finalR, finalG, finalB});

    verts.push_back({p1x, p1y, p1z, 0, 0, finalR, finalG, finalB});
    verts.push_back({p3x, p3y, p3z, 1, 1, finalR, finalG, finalB});
    verts.push_back({p4x, p4y, p4z, 0, 1, finalR, finalG, finalB});
  }
};
