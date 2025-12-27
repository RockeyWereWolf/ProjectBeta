import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Method;
import java.lang.reflect.Field;

public class DebugBiomeData {
    public static void main(String[] args) {
        long seed = -100L;
        World world = new World(new SaveHandler(new File("."), "DebugWorld", false),
                "DebugWorld", WorldProvider.getProviderForDimension(0), seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // 1. Initialize dependencies
            // We need real biome data this time, so we let WorldChunkManager run.
            // But we need to make sure generateTerrain uses the populated arrays.

            // ChunkProviderGenerate.provideChunk(0,0) calls loadBlockGeneratorData
            // which populates worldObj.getWorldChunkManager().temperature/humidity

            // We can just call provideChunk(0,0). It might crash on block saving but
            // the biome data should be populated before that.
            try {
                provider.provideChunk(0, 0);
            } catch (NullPointerException e) {
                // Ignore NPEs from block/biome array handling in later steps
            }

            // 2. Access the WorldChunkManager arrays
            double[] temps = world.getWorldChunkManager().temperature;
            double[] rains = world.getWorldChunkManager().humidity;

            // 3. Print the value at Index 17 (1,1)
            // This corresponds to Density Column (0,0)
            int idx = 1 * 16 + 1;

            System.out.println("=== JAVA BIOME DATA (Index 17) ===");
            if (temps != null && rains != null) {
                System.out.printf("Temp: %.17f\n", temps[idx]);
                System.out.printf("Rain: %.17f\n", rains[idx]);
            } else {
                System.out.println("Arrays are null!");
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}