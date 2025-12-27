# Project Beta

A C++ rewrite of Minecraft Beta 1.7.3 with rendering using Vulkan (via Diligent Engine). 

This is just a "for fun" project born out of boredom. The main goal right now is **maximum visual parity** with the original Java version world generation. It's not about being an optimized engine yet, it's about making it look and generate exactly like the old days.

## Important References (RetroMCP)

This project relies heavily on **[RetroMCP-Java](https://github.com/MCPHackers/RetroMCP-Java)**. 

*   All Java source code references, mapping names (like `func_4058_a`), and logic comparisons are based on the RetroMCP decompilation of Beta 1.7.3.
*   If you want to contribute or check the logic, you **need** to decompile the game using RetroMCP to understand what this project is matching against.

## Current State


### What Works / Matches Visually
*   **Terrain Shape:** The overall hills, mountains, and valleys look identical to the original game.
*   **Biome Map:** Biomes are seem to be placed correctly using the original lookup textures.
*   **Noise Math:** Ported the Perlin and Simplex noise algorithms, and they match Java's output to ~15 decimal places in isolation.

### Known Issues / In Progress
*   **The "Sandstone Offset":** In specific chunks (like 0,0), terrain density calculates slightly differently, putting the surface at `y=59` instead of `y=58`. This triggers a Sandstone generation check that Java doesn't do, which desynchronizes the RNG for the rest of the chunk. See note in **Debugging & Tools** to understand the to what chunks and heights I refer to.
*   **Decorations:**
    *   Grass/Sand transitions on beaches look a bit rougher/chaotic than vanilla.
    *   Clay is currently missing. Not implemented yet.
    *   Trees lack variety (no Big Trees implemented yet) and placement is sometimes off.
    *   Caves generate but have slightly different shapes (tighter entrances) and positions.

## Debugging & Tools

To help debug the generation, I've included a `src/tools/` folder.

*   **Java Debugs:** These are `.java` files that need to be compiled against the RetroMCP source. They dump raw values (RNG outputs, Noise arrays) from the original game.
*   **C++ Tests:** These are standalone C++ programs that run specific parts of the generation logic (like `TestDecisions.cpp`) to compare against the Java output.

**Note:** The current build is **hardcoded to Seed -100**. 
I used this seed for all programmatic parity checks. The `DebugWorld/` folder in this repo contains a saved vanilla Beta 1.7.3 world with this seed if you want to compare blocks manually. There is a `src/tools/WorldVerifier.cpp` that is coded to use this `DebugWorld`. If you don't want to debug this, you will need to consider changing `main.cpp` and `CMakeLists.txt` to ignore this source file and de-hardcode the seed.   

## The "Parity" Problem

Even with identical logic, the C++ build produces a slightly different terrain density than Java in the main project structure (`4 4 3 3` depth pattern vs `4 4 4 4`). 

What I tried:
*   Standardizing RNG (bit-perfect match).
*   Compiler flags (`-ffloat-store`, `-fno-fast-math`, `-O0`).
*   Architecture changes (stack vs heap allocation).

It might be a compiler bug, a weird optimization in GCC 15, or maybe I just missed something obvious. If you are a wizard with floating-point math or decompiled Java, feel free to take a look.

## My System Info

If you are trying to reproduce the build or the bugs, here is the machine I am building on:

*   **OS:** Arch Linux (Rolling)
*   **Kernel:** Linux (Standard Arch Kernel)
*   **Compiler:** GCC 15.2.1 (Prerelease/Snapshot) & Clang
*   **Build System:** CMake 3.31.2
*   **GPU:** NVIDIA GeForce RTX 2060

## Building

1.  **Clone recursively** (use DiligentEngine as a submodule):
    ```bash
    git clone --recursive https://github.com/RockeyWereWolf/ProjectBeta.git
    cd ProjectBeta
    ```

2.  **Build:**
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build . -- -j$(nproc)
    ```

3.  **Run:**
    ```bash
    ./ProjectBeta
    ```

---
*Credits to Mojang for the original game and DiligentGraphics for the rendering engine.*