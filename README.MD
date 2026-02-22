# Project Beta

A C++ rewrite of Minecraft Beta 1.7.3 with rendering using Vulkan (via Diligent Engine). 

This is just a "for fun" project born out of boredom. The main goal right now is **maximum visual parity** with the original Java version world generation. It's not about being an optimized engine yet, it's about making it look and generate exactly like the old days.

## Important References (RetroMCP)

This project relies heavily on **[RetroMCP-Java](https://github.com/MCPHackers/RetroMCP-Java)**. 

*   All Java source code references, mapping names (like `func_4058_a`), and logic comparisons are based on the RetroMCP decompilation of Beta 1.7.3.
*   If you want to contribute or check the logic, you **need** to decompile the game using RetroMCP to understand what this project is matching against.

## The Test Context (Seed -100)

**CRITICAL NOTE:** The current build is **hardcoded to Seed -100**. 

I used this specific seed as a scientific control for all programmatic parity checks. All observations below regarding specific coordinates, block heights, or "Sandstone anomalies" refer specifically to how this seed generates in the original game versus this port.

## Current State

### What Works / Matches Visually
*   **Terrain:** The overall hills, mountains, and valleys look identical to the original game.
*   **Biome Map:** Biomes seem to be placed correctly using the original lookup textures.
*   **Noise Math:** Ported the Perlin and Simplex noise algorithms, and they match Java's output to ~15 decimal places in isolation.
*   **RNG Sync (`4 4 3 3`):** Previous concerns about the Stone Depth sequence (`4 4 3 3`) being a bug were proved wrong. Detailed Java debugging (`tools/DebugRealStoneDepth.java`) confirmed that the original game produces this exact sequence when Sandstone generation consumes random numbers. The C++ implementation correctly matches this behavior.

### Known Issues / In Progress
*   **The "Sandstone/Surface Anomaly":** There is a persistent ambiguity regarding vertical alignment at specific coordinates (e.g., Chunk 0,0), depending on which ground truth you trust:
    *   **The Facts:** In this C++ clone, the surface is at `y=63`, Sandstone generates at `y=58`, and there are **5 blocks of sand**.
    *   **Java Observation:** In the real game, digging reveals **5 blocks of sand** above the sandstone.
    *   **The Ambiguity:**
        *   *If the WorldVerifier is correct:* It reports Java Sandstone at `y=58`. This implies Java Surface is `63` (58+5). In this case, **both Surface and Sandstone heights match perfectly**.
        *   *If [Minecraft Forums](https://www.minecraftforum.net/forums/minecraft-java-edition/survival-mode/220241-quick-question-about-f3) (F3 logic) are correct:* F3 indicates standing on block `y=59`. This implies Java Surface is `64` (59+5). In this case, **the C++ terrain generated 1 block too low**.(Messages from FinalMinuet)

    **Caves:** Math and RNG are verified to match Java, but rasterization edge-cases (floating point flooring) cause some caves to generate their floor 1 block deeper or slightly wider than the original game.
        
*   **Decorations:**
    *   Clay is currently missing. Not implemented yet.
    *   Trees lack variety (no Big Trees implemented yet) and placement is sometimes off.

## Debugging & Tools

To help debug the generation, I've included a `src/tools/` folder.

*   **Java Debugs:** These are `.java` files that need to be compiled against the RetroMCP source. They dump raw values (RNG outputs, Noise arrays) from the original game.
*   **C++ Tests:** These are standalone C++ programs that run specific parts of the generation logic (like `TestDecisions.cpp`) to compare against the Java output.

**Note on Verification:**
The `src/tools/WorldVerifier.cpp` tool is hardcoded to look for region files in a specific path on my local machine. The `DebugWorld` folder in this repo is currently empty. If you want to run the verifier, you must provide your own Beta 1.7.3 region files and update the path in the code.

## The RNG Depth Desync Problem

I spent days chasing a ghost regarding floating-point determinism and compiler behavior. 

I initially thought the RNG was desynchronizing because this C++ project output a Stone Depth pattern of `4 4 3 3` while the isolated test tool output `4 4 4 4`. I blamed everything: GCC optimizations, `-ffast-math`, stack vs heap allocation, and even the OS kernel.

**The Twist:**
It turned out the "failing" main project was actually **correct**. 
*   The isolated test (`TestDecisions.cpp`) used a Mock Chunk filled with stone, which prevented "Sandstone" generation logic from ever running. This caused it to skip an RNG call, yielding the "clean" `4 4 4 4` pattern.
*   The main project generated real terrain, which triggered Sandstone, consumed the RNG, and produced `4 4 3 3`.
*   I verified that Java **also** produces `4 4 3 3` when Sandstone generates.

So the code is logically consistent. The remaining discrepancies (like the potential 1-block surface offset) are likely due to microscopic floating-point accumulation errors in the trilinear interpolation loop that differ between the JVM's `strictfp` and C++ binaries, or simply a logic difference I haven't spotted yet.

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

## License

This project is licensed under the GPL-3.0 License - see the [LICENSE](LICENSE) file for details.

---
*Credits to Mojang for the original game and DiligentGraphics for the rendering engine.*