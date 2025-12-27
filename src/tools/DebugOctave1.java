import net.minecraft.src.*;
import java.lang.reflect.Field;
import java.util.Random;

public class DebugOctave1 {
    public static void main(String[] args) {
        long seed = -100L;
        Random random = new Random(seed);

        // Octave 0 (Consume and ignore)
        new NoiseGeneratorPerlin(random);

        // Octave 1 (Target)
        NoiseGeneratorPerlin gen1 = new NoiseGeneratorPerlin(random);

        try {
            Field xF = NoiseGeneratorPerlin.class.getDeclaredField("xCoord");
            Field yF = NoiseGeneratorPerlin.class.getDeclaredField("yCoord");
            Field zF = NoiseGeneratorPerlin.class.getDeclaredField("zCoord");
            Field permF = NoiseGeneratorPerlin.class.getDeclaredField("permutations");
            xF.setAccessible(true);
            yF.setAccessible(true);
            zF.setAccessible(true);
            permF.setAccessible(true);

            System.out.println("=== JAVA OCTAVE 1 CHECK ===");
            System.out.printf("x: %.20f\n", xF.getDouble(gen1));
            System.out.printf("y: %.20f\n", yF.getDouble(gen1));
            System.out.printf("z: %.20f\n", zF.getDouble(gen1));
            int[] p = (int[]) permF.get(gen1);
            System.out.println("Perm[0]: " + p[0]);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}