import net.minecraft.src.*;
import java.lang.reflect.Field;
import java.util.Random;

public class DebugScaleInit {
    public static void main(String[] args) {
        long seed = -100L;
        Random random = new Random(seed);

        // Initialize the generators in the exact order of ChunkProviderGenerate
        new NoiseGeneratorOctaves(random, 16); // minLimit (field_705_k)
        new NoiseGeneratorOctaves(random, 16); // maxLimit (field_704_l)
        new NoiseGeneratorOctaves(random, 8); // main (field_703_m)
        new NoiseGeneratorOctaves(random, 4); // surface (field_702_n)
        new NoiseGeneratorOctaves(random, 4); // val (field_701_o)

        // The next one is scaleNoise (field_715_a), which has 10 octaves.
        // We capture its first octave.
        NoiseGeneratorPerlin scaleOct0 = new NoiseGeneratorPerlin(random);

        try {
            Field xF = NoiseGeneratorPerlin.class.getDeclaredField("xCoord");
            xF.setAccessible(true);
            System.out.println("=== JAVA SCALE NOISE INIT CHECK ===");
            System.out.printf("xCoord: %.20f\n", xF.getDouble(scaleOct0));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}