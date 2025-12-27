import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Method;

public class DebugDensityOutput {
    public static void main(String[] args) {
        long seed = -100L;
        // Minimal setup to satisfy constructor
        World world = new World(new SaveHandler(new File("."), "DebugWorld", false),
                "DebugWorld", WorldProvider.getProviderForDimension(0), seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // 1. Inject dummy data to prevent NullPointerException
            double[] dummy = new double[16 * 16];
            world.getWorldChunkManager().temperature = dummy;
            world.getWorldChunkManager().humidity = dummy;

            // 2. Find func_4058_a (generateTerrain) via Reflection
            Method genTerrain = null;
            for (Method m : ChunkProviderGenerate.class.getDeclaredMethods()) {
                if (m.getParameterCount() == 7 && m.getReturnType() == double[].class) {
                    genTerrain = m;
                    break;
                }
            }
            if (genTerrain == null) {
                System.out.println("Error: func_4058_a not found");
                return;
            }
            genTerrain.setAccessible(true);

            // 3. Invoke for Chunk 0,0
            // Params: (noiseArray, x, y, z, sizeX, sizeY, sizeZ)
            // x=0, y=0, z=0. Sizes: 5, 17, 5.
            double[] densities = (double[]) genTerrain.invoke(provider, null, 0, 0, 0, 5, 17, 5);

            System.out.println("=== JAVA DENSITY OUTPUT CHECK ===");
            // Print densities for the first column (x=0, z=0) at various Y levels
            // Index logic: ((x * sizeZ + z) * sizeY + y)
            // Column 0 starts at index 0.

            System.out.printf("Density[0] (y=0):  %.20f\n", densities[0]);
            System.out.printf("Density[7] (y=7):  %.20f\n", densities[7]);
            System.out.printf("Density[8] (y=8):  %.20f\n", densities[8]);
            System.out.printf("Density[9] (y=9):  %.20f\n", densities[9]);
            System.out.printf("Density[16] (y=16): %.20f\n", densities[16]);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}