import net.minecraft.src.*;
import java.util.Random;
import java.lang.reflect.Method;

public class DebugOctave0GenReflection {
    public static void main(String[] args) {
        try {
            long seed = -100L;
            Random random = new Random(seed);

            // Create first octave
            NoiseGeneratorPerlin oct0 = new NoiseGeneratorPerlin(random);
            double[] noise = new double[5 * 17 * 5];

            // Use Reflection to find func_646_a by name and argument count
            Method targetMethod = null;
            for (Method m : NoiseGeneratorPerlin.class.getMethods()) {
                if (m.getName().equals("func_646_a") && m.getParameterCount() == 11) {
                    targetMethod = m;
                    break;
                }
            }

            if (targetMethod == null) {
                System.out.println("ERROR: Could not find func_646_a via reflection!");
                return;
            }

            // Invoke: (array, x, y, z, sizeX, sizeY, sizeZ, scaleX, scaleY, scaleZ, amp)
            targetMethod.invoke(oct0,
                    noise,
                    0.0D, 0.0D, 0.0D,
                    5, 17, 5,
                    684.412D, 684.412D, 684.412D,
                    1.0D);

            System.out.println("=== JAVA OCTAVE 0 GEN CHECK ===");
            System.out.printf("idx 0: %.20f\n", noise[0]);
            System.out.printf("idx 1: %.20f\n", noise[1]);
            System.out.printf("idx 2: %.20f\n", noise[2]);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}