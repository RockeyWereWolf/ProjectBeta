import net.minecraft.src.*;
import java.io.File;

public class DebugBiomes {
    public static void main(String[] args) {
        long seed = -100L;
        // The ISaveHandler is necessary for the World constructor, as you noted.
        ISaveHandler saveHandler = new SaveHandler(new File("."), "DebugWorld", false);

        WorldProvider worldProvider = WorldProvider.getProviderForDimension(0);
        World world = new World(saveHandler, "DebugWorld", worldProvider, seed);

        System.out.println("=== BIOME CHECK (Seed " + seed + ") ===");

        WorldChunkManager chunkManager = world.getWorldChunkManager();

        // This array is just a dummy to pass to the function, the results will be in
        // public fields.
        BiomeGenBase[] biomeBuffer = new BiomeGenBase[1];

        // Check the first 4 blocks along the X axis
        for (int x = 0; x < 4; x++) {
            // This is the correct way to get the data for a single block at (x, 0)
            chunkManager.loadBlockGeneratorData(biomeBuffer, x, 0, 1, 1);

            // The results are now in the public member arrays of the chunkManager
            BiomeGenBase b = biomeBuffer[0];
            double t = chunkManager.temperature[0];
            double r = chunkManager.humidity[0]; // In this version, it's called humidity

            // There is no biomeID field in Beta 1.7.3's BiomeGenBase
            System.out.println("Biome at (" + x + ", 0): " + b.biomeName);
            System.out.printf("   Temp: %.6f, Rain: %.6f\n", t, r);
        }
    }
}