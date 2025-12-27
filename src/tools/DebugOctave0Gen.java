import net.minecraft.src.*;
import java.util.Random;

public class DebugOctave0Gen {
    public static void main(String[] args) {
        long seed = -100L;
        Random random = new Random(seed);

        // This generates the first PerlinNoise instance (Octave 0)
        NoiseGeneratorPerlin oct0 = new NoiseGeneratorPerlin(random);

        double[] noise = new double[5 * 17 * 5];

        // Call func_805_a (which matches populateNoiseArray)
        oct0.func_805_a(noise, 0.0D, 0.0D, 0.0D, 5, 17, 5, 684.412D, 684.412D, 684.412D, 1.0D);

        System.out.println("=== JAVA OCTAVE 0 GEN CHECK ===");
        System.out.printf("idx 0: %.20f\n", noise[0]);
        System.out.printf("idx 1: %.20f\n", noise[1]);
        System.out.printf("idx 2: %.20f\n", noise[2]);
        // Also check mid-point to verify Y/Z looping
        int midIdx = (0 * 5 + 0) * 17 + 8;
        System.out.printf("idx %d: %.20f\n", midIdx, noise[midIdx]);
    }
}