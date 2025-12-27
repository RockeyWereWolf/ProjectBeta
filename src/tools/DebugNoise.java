import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;

public class DebugNoise {
    public static void main(String[] args) {
        long seed = -100L;

        // 1. Setup the World and ChunkProvider
        // We use the same setup as your DebugBiomes to ensure the context is right
        ISaveHandler saveHandler = new SaveHandler(new File("."), "DebugWorld", false);
        WorldProvider worldProvider = WorldProvider.getProviderForDimension(0);
        World world = new World(saveHandler, "DebugWorld", worldProvider, seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // 2. Access the private 'sandNoise' array via reflection
            // In ChunkProviderGenerate, this is the array used for sand generation
            Field sandNoiseField = ChunkProviderGenerate.class.getDeclaredField("sandNoise");
            sandNoiseField.setAccessible(true);

            // 3. Trigger the generation for Chunk 0,0
            // This calls replaceBlocksForBiome internally
            provider.provideChunk(0, 0);

            // 4. Read the values
            double[] sandNoise = (double[]) sandNoiseField.get(provider);

            System.out.println("=== JAVA NOISE CHECK (Chunk 0,0) ===");
            System.out.println("Seed: " + seed);
            System.out.println("First 5 values of sandNoise:");
            for (int i = 0; i < 5; i++) {
                // Print with high precision
                System.out.printf("idx %d: %.20f\n", i, sandNoise[i]);
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}