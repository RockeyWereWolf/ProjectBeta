import net.minecraft.src.*;
import java.util.Random;
import java.lang.reflect.Method;

public class DebugSimplex {
    public static void main(String[] args) {
        long seed = -100L;
        Random random = new Random(seed);

        // Simplex Noise Generator
        NoiseGenerator2 simplex = new NoiseGenerator2(random);

        double[] noise = new double[1];

        try {
            // Find func_4115_a via reflection to avoid symbol errors
            Method genMethod = null;
            for (Method m : NoiseGenerator2.class.getDeclaredMethods()) {
                // Look for method with 8 parameters: (double[], double, double, int, int,
                // double, double, double)
                if (m.getParameterCount() == 8 && m.getParameterTypes()[0] == double[].class) {
                    genMethod = m;
                    break;
                }
            }

            if (genMethod == null) {
                System.out.println("ERROR: Could not find Simplex generation method!");
                return;
            }

            // Testing at coords 0,0 with scale 1.0, amp 1.0
            // Params: array, x, y, sizeX, sizeY, scaleX, scaleY, amp
            genMethod.invoke(simplex, noise, 0.5D, 0.5D, 1, 1, 1.0D, 1.0D, 1.0D);

            System.out.println("=== JAVA SIMPLEX CHECK ===");
            System.out.printf("Value: %.20f\n", noise[0]);

            Field stoneNoiseF = ChunkProviderGenerate.class.getDeclaredField("field_4183_f");
            stoneNoiseF.setAccessible(true);
            double[] stoneNoise = (double[]) stoneNoiseF.get(provider);

            System.out.println("=== JAVA STONE NOISE CHECK ===");
            for (int i = 0; i < 4; i++)
                System.out.printf("idx %d: %.20f\n", i, stoneNoise[i]);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}