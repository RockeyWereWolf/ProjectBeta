import net.minecraft.src.*;
import java.util.Random;
import java.io.File;

public class DebugChunkGen {
    public static void main(String[] args) {
        System.out.println("=== DEBUGGING CHUNK 0,0 GENERATION ===");
        long seed = -100L;

        // 1. Create dummy objects required by the World constructor

        // The World needs a save handler, even if we don't save.
        // SaveHandler is a concrete implementation of ISaveHandler.
        ISaveHandler saveHandler = new SaveHandler(new File("."), "DebugWorld", false);

        // The World needs a provider (0 = Overworld). This creates a
        // WorldProviderSurface.
        WorldProvider worldProvider = WorldProvider.getProviderForDimension(0);

        // 2. Create the World object using the correct constructor
        World world = new World(saveHandler, "DebugWorld", worldProvider, seed);

        // 3. Instantiate the Generator (Your original code is correct from here)
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        // 4. Hijack provideChunk logic to inspect state
        // We manually do what provideChunk(0, 0) does:

        int chunkX = 0;
        int chunkZ = 0;

        // REPLICATE: Seed Calculation
        long calculatedSeed = (long) chunkX * 341873128712L + (long) chunkZ * 132897987541L;
        System.out.println("Calculated Chunk Seed: " + calculatedSeed);

        Random r = new Random(calculatedSeed);
        System.out.println("Test Random(Seed) -> nextDouble: " + r.nextDouble());

        // 5. Run the Actual Generation
        // This will fill the byte array using the game's actual noise and logic

        BiomeGenBase biome = world.getWorldChunkManager().getBiomeGenAt(0, 0);
        System.out.println("Biome at 0,0: " + biome.biomeName);

        Chunk chunk = provider.provideChunk(chunkX, chunkZ);

        // 6. Inspect Block at 0, 1, 0 (Where mismatch is)
        int blockID = chunk.getBlockID(0, 1, 0);
        String blockName = (blockID > 0 && blockID < Block.blocksList.length && Block.blocksList[blockID] != null)
                ? Block.blocksList[blockID].getBlockName()
                : "Unknown";
        System.out.println("Block at 0,1,0: " + blockID + " (" + blockName + ")");

        // ... the rest of your logic ...
    }
}