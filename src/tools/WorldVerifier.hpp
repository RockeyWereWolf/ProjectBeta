#pragma once

#include "../world/Chunk.hpp"
#include "../world/WorldGen.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

// We need zlib. Assuming it's available via include paths.
// If not, we might need to adjust include.
#include <zlib.h>

/*
 * Simple Region File (.mcr) Reader and NBT Parser for "Blocks"
 * Beta 1.7.3 format.
 */

namespace ParityVerifier {

struct RegionChunkData {
  std::vector<uint8_t> blocks; // 32768 bytes
  bool isValid = false;
};

// Helper to decompress ZLib data
static std::vector<uint8_t> decompress(const std::vector<uint8_t> &compressed) {
  std::vector<uint8_t> out;
  out.resize(1024 * 1024); // 1MB buffer should be enough for a chunk

  z_stream zs;
  zs.zalloc = Z_NULL;
  zs.zfree = Z_NULL;
  zs.opaque = Z_NULL;
  zs.avail_in = (uInt)compressed.size();
  zs.next_in = (Bytef *)compressed.data();
  zs.avail_out = (uInt)out.size();
  zs.next_out = (Bytef *)out.data();

  if (inflateInit(&zs) != Z_OK)
    return {};

  int ret = inflate(&zs, Z_FINISH);
  if (ret != Z_STREAM_END && ret != Z_OK && ret != Z_BUF_ERROR) {
    inflateEnd(&zs);
    return {};
  }

  out.resize(zs.total_out);
  inflateEnd(&zs);
  return out;
}

// Naive NBT Scanner to find "Blocks" Byte Array
static std::vector<uint8_t>
extractBlocksFromNBT(const std::vector<uint8_t> &nbt) {
  // We look for Tag Type 7 (ByteArray) with name "Blocks"
  // Pattern: 07 00 06 'B' 'l' 'o' 'c' 'k' 's' [Int32 Length] [Data]

  static const uint8_t searchPattern[] = {0x07, 0x00, 0x06, 'B', 'l',
                                          'o',  'c',  'k',  's'};

  auto it = std::search(nbt.begin(), nbt.end(), std::begin(searchPattern),
                        std::end(searchPattern));
  if (it == nbt.end())
    return {};

  // Advance past pattern
  size_t offset = std::distance(nbt.begin(), it) + sizeof(searchPattern);

  // Read Length (Big Endian Int32)
  if (offset + 4 > nbt.size())
    return {};

  uint32_t len = (nbt[offset] << 24) | (nbt[offset + 1] << 16) |
                 (nbt[offset + 2] << 8) | nbt[offset + 3];
  offset += 4;

  if (len != 32768) {
    // Beta 1.7.3 chunks must be 32768 bytes
    // If not, maybe something is wrong or it's a newer format
    // printf("Found Blocks tag but len is %d\n", len);
    return {};
  }

  if (offset + len > nbt.size())
    return {};

  // Copy data
  std::vector<uint8_t> blocks(len);
  std::copy(nbt.begin() + offset, nbt.begin() + offset + len, blocks.begin());
  return blocks;
}

class RegionLoader {
public:
  static RegionChunkData loadChunk(const std::string &mcrPath, int localX,
                                   int localZ) {
    FILE *f = fopen(mcrPath.c_str(), "rb");
    if (!f)
      return {};

    // Offset is at 4 * (localX + localZ * 32)
    int offsetIdx = localX + localZ * 32;
    fseek(f, offsetIdx * 4, SEEK_SET);

    uint8_t loc[4];
    if (fread(loc, 1, 4, f) != 4) {
      fclose(f);
      return {};
    }

    uint32_t offset = (loc[0] << 16) | (loc[1] << 8) | loc[2];
    uint8_t sectorCount = loc[3];

    if (offset == 0) {
      fclose(f);
      return {};
    } // Not generated

    fseek(f, offset * 4096, SEEK_SET);

    // Read Chunk Header (Length + Compression)
    uint8_t header[5];
    if (fread(header, 1, 5, f) != 5) {
      fclose(f);
      return {};
    }

    uint32_t length =
        (header[0] << 24) | (header[1] << 16) | (header[2] << 8) | header[3];
    uint8_t compression = header[4];
    length -= 1; // Subtract compression byte from length count

    // Read Compressed Data
    std::vector<uint8_t> compressed(length);
    if (fread(compressed.data(), 1, length, f) != length) {
      fclose(f);
      return {};
    }
    fclose(f);

    // Decompress
    // Minecraft uses GZip (1) or ZLib (2). Usually 2 in Region files.
    // If 1, inflate handles it anyway mostly? ZLib/Gzip headers differ.
    // Beta 1.7.3 MCR usually Type 2 (ZLib).

    std::vector<uint8_t> nbt = decompress(compressed);
    if (nbt.empty())
      return {};

    std::vector<uint8_t> blocks = extractBlocksFromNBT(nbt);

    RegionChunkData data;
    data.blocks = blocks;
    data.isValid = !blocks.empty();
    return data;
  }
};

class Verifier {
public:
  static void verify(const std::string &regionPath,
                     const std::vector<Chunk> &chunks, int regionX,
                     int regionZ) {

    printf("[Verifier] Checking against Region: %s\n", regionPath.c_str());

    int verifiedCount = 0;

    for (const auto &chunk : chunks) {
      int cx = chunk.chunkX;
      int cz = chunk.chunkZ;
      int myRX = cx >> 5;
      int myRZ = cz >> 5;

      bool inRegion = (cx >= regionX * 32 && cx < (regionX + 1) * 32 &&
                       cz >= regionZ * 32 && cz < (regionZ + 1) * 32);

      if (!inRegion)
        continue;

      int localX = cx & 31;
      int localZ = cz & 31;

      RegionChunkData original =
          RegionLoader::loadChunk(regionPath, localX, localZ);
      if (!original.isValid) {
        continue;
      }

      // Compare
      int mismatch = 0;
      int firstMismatchId = -1;

      for (int y = 0; y < 128; ++y) {
        for (int z = 0; z < 16; ++z) {
          for (int x = 0; x < 16; ++x) {
            int valIdx = (x * 16 + z) * 128 + y;

            uint8_t expected = original.blocks[valIdx];
            uint8_t actual = chunk.getBlock(x, y, z);

            if (expected != actual) {
              mismatch++;
              if (firstMismatchId == -1)
                firstMismatchId = expected;

              if (mismatch <= 3) {
                printf(
                    "Mismatch Chunk [%d,%d] at %d,%d,%d: Expected %d, Got %d\n",
                    cx, cz, x, y, z, expected, actual);
              }
            }
          }
        }
      }

      if (mismatch == 0) {
        printf("[Verifier] Chunk [%d, %d] MATCHES PERFECTLY!\n", cx, cz);
      } else {
        printf("[Verifier] Chunk [%d, %d] has %d mismatches. (First Exp: %d)\n",
               cx, cz, mismatch, firstMismatchId);
      }
      verifiedCount++;
    }

    if (verifiedCount == 0) {
      printf("[Verifier] Warning: No chunks in the list matched the region "
             "coordinates (0..31).\n");
    }
  }
};
} // namespace ParityVerifier
