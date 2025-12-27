import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class DebugDensityInputsSafe {
    public static void main(String[] args) {
        long seed = -100L;
        // Setup minimal world
        World world = new World(new SaveHandler(new File("."), "DebugWorld", false),
                "DebugWorld", WorldProvider.getProviderForDimension(0), seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // 1. Inject dummy temp/rain arrays to prevent NPE in func_4058_a
            double[] dummyTemp = new double[16 * 16];
            double[] dummyRain = new double[16 * 16];
            world.getWorldChunkManager().temperature = dummyTemp;
            world.getWorldChunkManager().humidity = dummyRain;

            // 2. Find and Call func_4058_a (generateTerrain) via Reflection
            // This bypasses the crash in provideChunk
            Method genTerrain = null;
            for (Method m : ChunkProviderGenerate.class.getDeclaredMethods()) {
                // Look for the method signature: returns double[], takes 7 params
                if (m.getParameterCount() == 7 && m.getReturnType() == double[].class) {
                    genTerrain = m;
                    break;
                }
            }

            if (genTerrain == null) {
                System.out.println("Error: Could not find func_4058_a (generateTerrain)");
                return;
            }
            genTerrain.setAccessible(true);

            // 3. Invoke it for Chunk 0,0
            // Params: (noiseArray, x, y, z, sizeX, sizeY, sizeZ) -> (null, 0, 0, 0, 5, 17,
            // 5)
            genTerrain.invoke(provider, null, 0, 0, 0, 5, 17, 5);

            // 4. Access the Noise Arrays using the names PROVEN by DebugFieldList
            Field scaleNoiseF = ChunkProviderGenerate.class.getDeclaredField("field_4182_g");
            Field depthNoiseF = ChunkProviderGenerate.class.getDeclaredField("field_4181_h");

            scaleNoiseF.setAccessible(true);
            depthNoiseF.setAccessible(true);

            double[] scaleNoise = (double[]) scaleNoiseF.get(provider);
            double[] depthNoise = (double[]) depthNoiseF.get(provider);

            // 5. Print results for Index 0
            System.out.println("=== JAVA DENSITY INPUTS (0,0) ===");
            System.out.printf("Scale Noise: %.17f\n", scaleNoise[0]);
            System.out.printf("Depth Noise: %.17f\n", depthNoise[0]);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}