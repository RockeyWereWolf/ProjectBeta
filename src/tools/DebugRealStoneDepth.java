import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;
import java.util.Random;

public class DebugRealStoneDepth {
    public static void main(String[] args) {
        long seed = -100L;
        World world = new World(new SaveHandler(new File("."), "DebugWorld", false),
                "DebugWorld", WorldProvider.getProviderForDimension(0), seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // Trigger generation
            try {
                provider.provideChunk(0, 0);
            } catch (Exception e) {
            }

            // Extract Arrays
            Field sandF = ChunkProviderGenerate.class.getDeclaredField("sandNoise");
            Field stoneF = ChunkProviderGenerate.class.getDeclaredField("stoneNoise");
            sandF.setAccessible(true);
            stoneF.setAccessible(true);
            double[] sandNoise = (double[]) sandF.get(provider);
            double[] stoneNoise = (double[]) stoneF.get(provider);

            // Extract RNG
            Random random = new Random((long) 0 * 341873128712L + (long) 0 * 132897987541L);

            System.out.println("=== JAVA REAL STONE DEPTHS & NOISE ===");

            for (int x = 0; x < 4; ++x) {
                int idx = x;

                double sandRnd = random.nextDouble();
                boolean genSand = sandNoise[idx] + sandRnd * 0.2 > 0.0;
                double gravelRnd = random.nextDouble();

                double stoneRnd = random.nextDouble();
                int stoneDepth = (int) (stoneNoise[idx] / 3.0 + 3.0 + stoneRnd * 0.25);

                // --- ADDED DETAILED PRINT ---
                System.out.printf("Col %d: Noise[D]: %.17f Rnd: %.17f -> Depth: %d\n",
                        x, stoneNoise[idx], stoneRnd, stoneDepth);

                // Bedrock Loop
                for (int i = 0; i < 128; i++)
                    random.nextInt(5);

                // Simulate Sandstone RNG consumption for Col 0 (matching your C++ observation)
                if (x == 0) {
                    System.out.println(" (Simulating Col 0 Sandstone consumption)");
                    random.nextInt(4);
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}