import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;

public class DebugStoneNoise {
    public static void main(String[] args) {
        long seed = -100L;
        // 1. Setup World and Provider
        ISaveHandler saveHandler = new SaveHandler(new File("."), "DebugWorld", false);
        WorldProvider worldProvider = WorldProvider.getProviderForDimension(0);
        World world = new World(saveHandler, "DebugWorld", worldProvider, seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // 2. Call provideChunk to trigger the internal noise generation.
            // We can ignore the crash that happens later in the method.
            try {
                provider.provideChunk(0, 0);
            } catch (Exception e) {
                // This is expected because we haven't set up a full world,
                // but the noise arrays will be populated by this point.
            }

            // 3. Access the 'stoneNoise' field using its correct name from your field list.
            Field stoneNoiseField = ChunkProviderGenerate.class.getDeclaredField("stoneNoise");
            stoneNoiseField.setAccessible(true);

            // 4. Get the array from the provider instance
            double[] stoneNoise = (double[]) stoneNoiseField.get(provider);

            System.out.println("=== JAVA STONE NOISE CHECK ===");
            for (int i = 0; i < 4; i++) {
                System.out.printf("idx %d: %.20f\n", i, stoneNoise[i]);
            }

        } catch (Exception e) {
            System.out.println("An error occurred during reflection:");
            e.printStackTrace();
        }
    }
}