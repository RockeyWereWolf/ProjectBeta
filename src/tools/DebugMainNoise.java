import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class DebugMainNoise {
    public static void main(String[] args) {
        long seed = -100L;
        // 1. Setup World
        World world = new World(new SaveHandler(new File("."), "DebugWorld", false),
                "DebugWorld", WorldProvider.getProviderForDimension(0), seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // FIX: Initialize the temperature/humidity arrays in WorldChunkManager
            // The generator reads these, so they must not be null.
            double[] dummyStats = new double[16 * 16];
            world.getWorldChunkManager().temperature = dummyStats;
            world.getWorldChunkManager().humidity = dummyStats;

            // 2. Find generateTerrain (func_4058_a)
            Method genTerrain = null;
            // Look for the method with 7 parameters returning double[]
            for (Method m : ChunkProviderGenerate.class.getDeclaredMethods()) {
                if (m.getParameterCount() == 7 && m.getReturnType() == double[].class) {
                    genTerrain = m;
                    break;
                }
            }
            if (genTerrain == null) {
                System.out.println("Error: Could not find generateTerrain method");
                return;
            }
            genTerrain.setAccessible(true);

            // 3. Invoke it for Chunk 0,0
            // Params: (noiseArray, x, y, z, sizeX, sizeY, sizeZ)
            genTerrain.invoke(provider, null, 0, 0, 0, 5, 17, 5);

            // 4. Access Main Noise (field_4185_d)
            // Using the name from your DebugFieldList output
            Field mainNoiseF = ChunkProviderGenerate.class.getDeclaredField("field_4185_d");
            mainNoiseF.setAccessible(true);

            double[] noise = (double[]) mainNoiseF.get(provider);

            System.out.println("=== JAVA MAIN NOISE CHECK ===");
            System.out.printf("idx 0: %.20f\n", noise[0]);
            System.out.printf("idx 8: %.20f\n", noise[8]);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}