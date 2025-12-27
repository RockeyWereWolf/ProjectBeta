import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;

public class Debug3DNoise {
    public static void main(String[] args) {
        long seed = -100L;
        World world = new World(new SaveHandler(new File("."), "DebugWorld", false),
                "DebugWorld", WorldProvider.getProviderForDimension(0), seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            provider.provideChunk(0, 0);

            // Access field_4259_a (which corresponds to minLimitNoise output array in
            // generateTerrain)
            // Wait, we need to map the field names.
            // minLimitNoise -> field_922_a -> generates into field_4260_b (minNoiseArray)
            // But provideChunk calls func_4063_a which fills these arrays.
            // We need to access the array AFTER it has been filled.

            // Actually, we can just use the NoiseGeneratorOctaves directly to be sure.
            // field_922_a is minLimitNoise.
            Field minNoiseGenF = ChunkProviderGenerate.class.getDeclaredField("field_922_a");
            minNoiseGenF.setAccessible(true);
            NoiseGeneratorOctaves minGen = (NoiseGeneratorOctaves) minNoiseGenF.get(provider);

            double[] minNoiseArray = new double[5 * 17 * 5];

            minGen.generateNoiseOctaves(minNoiseArray, 0, 0, 0, 5, 17, 5,
                    684.412, 684.412, 684.412);

            System.out.println("=== JAVA 3D NOISE CHECK ===");
            for (int i = 0; i < 5; i++) {
                System.out.printf("idx %d: %.20f\n", i, minNoiseArray[i]);
            }
            int midIdx = (0 * 5 + 0) * 17 + 8;
            System.out.printf("idx %d: %.20f\n", midIdx, minNoiseArray[midIdx]);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}