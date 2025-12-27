import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;
import java.util.Random;

public class DebugPermutations {
    public static void main(String[] args) {
        long seed = -100L;

        // Replicate the exact initialization of WorldGen/ChunkProviderGenerate
        Random random = new Random(seed);

        // The first generator initialized is minLimitNoise (16 octaves)
        // Its first octave is the first PerlinNoise created.
        NoiseGeneratorPerlin firstPerlin = new NoiseGeneratorPerlin(random);

        try {
            Field xF = NoiseGeneratorPerlin.class.getDeclaredField("xCoord");
            Field yF = NoiseGeneratorPerlin.class.getDeclaredField("yCoord");
            Field zF = NoiseGeneratorPerlin.class.getDeclaredField("zCoord");
            Field permF = NoiseGeneratorPerlin.class.getDeclaredField("permutations");

            xF.setAccessible(true);
            yF.setAccessible(true);
            zF.setAccessible(true);
            permF.setAccessible(true);

            System.out.println("=== JAVA PERLIN GEN 0 CHECK ===");
            System.out.printf("xCoord: %.20f\n", xF.getDouble(firstPerlin));
            System.out.printf("yCoord: %.20f\n", yF.getDouble(firstPerlin));
            System.out.printf("zCoord: %.20f\n", zF.getDouble(firstPerlin));

            int[] perm = (int[]) permF.get(firstPerlin);
            System.out.print("Perm[0..9]: ");
            for (int i = 0; i < 10; i++)
                System.out.print(perm[i] + " ");
            System.out.println();

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}