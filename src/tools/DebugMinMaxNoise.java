import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class DebugMinMaxNoise {
    public static void main(String[] args) {
        long seed = -100L;
        World world = new World(new SaveHandler(new File("."), "DebugWorld", false),
                "DebugWorld", WorldProvider.getProviderForDimension(0), seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // Initialize dummy biome data
            double[] dummy = new double[16 * 16];
            world.getWorldChunkManager().temperature = dummy;
            world.getWorldChunkManager().humidity = dummy;

            // Generate terrain arrays
            Method genTerrain = null;
            for (Method m : ChunkProviderGenerate.class.getDeclaredMethods()) {
                if (m.getParameterCount() == 7 && m.getReturnType() == double[].class) {
                    genTerrain = m;
                    break;
                }
            }
            if (genTerrain != null) {
                genTerrain.setAccessible(true);
                genTerrain.invoke(provider, null, 0, 0, 0, 5, 17, 5);
            }

            // Access field_4228_e (Min Noise) and field_4227_f (Max Noise)
            // Based on DebugFieldList order:
            // field_4185_d -> Main (Verified)
            // field_4184_e -> Min ?
            // field_4183_f -> Max ?

            Field minF = ChunkProviderGenerate.class.getDeclaredField("field_4184_e");
            Field maxF = ChunkProviderGenerate.class.getDeclaredField("field_4183_f");
            minF.setAccessible(true);
            maxF.setAccessible(true);

            double[] minN = (double[]) minF.get(provider);
            double[] maxN = (double[]) maxF.get(provider);

            System.out.println("=== JAVA MIN/MAX NOISE CHECK ===");
            System.out.printf("Min[0]: %.20f\n", minN[0]);
            System.out.printf("Max[0]: %.20f\n", maxN[0]);
            System.out.printf("Min[8]: %.20f\n", minN[8]); // Middle Y
            System.out.printf("Max[8]: %.20f\n", maxN[8]);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}