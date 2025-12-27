import net.minecraft.src.*;
import java.util.Random;
import java.lang.reflect.Method;

public class DebugOctave1Gen {
    public static void main(String[] args) {
        try {
            long seed = -100L;
            Random random = new Random(seed);

            // Burn Octave 0
            new NoiseGeneratorPerlin(random);
            // Create Octave 1
            NoiseGeneratorPerlin oct1 = new NoiseGeneratorPerlin(random);

            double[] noise = new double[5 * 17 * 5];

            Method targetMethod = null;
            for (Method m : NoiseGeneratorPerlin.class.getMethods()) {
                // Look for func_805_a or equivalent with 11 params
                if (m.getParameterCount() == 11) {
                    targetMethod = m;
                    break;
                }
            }

            // Parameters for Octave 1: Scales are halved, Amp is 0.5
            double scale = 684.412D * 0.5D;
            double amp = 0.5D;

            targetMethod.invoke(oct1, noise, 0.0D, 0.0D, 0.0D, 5, 17, 5, scale, scale, scale, amp);

            System.out.println("=== JAVA OCTAVE 1 GEN CHECK ===");
            System.out.printf("idx 0: %.20f\n", noise[0]);
            System.out.printf("idx 1: %.20f\n", noise[1]);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}