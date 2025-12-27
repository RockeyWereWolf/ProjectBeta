import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;
import java.util.Random;

public class DebugDecisions {
    public static void main(String[] args) {
        long seed = -100L;
        ISaveHandler saveHandler = new SaveHandler(new File("."), "DebugWorld", false);
        WorldProvider worldProvider = WorldProvider.getProviderForDimension(0);
        World world = new World(saveHandler, "DebugWorld", worldProvider, seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // 1. Generate the chunk to populate private fields
            provider.provideChunk(0, 0);

            // 2. Access private fields
            Field sandNoiseF = ChunkProviderGenerate.class.getDeclaredField("sandNoise");
            Field gravelNoiseF = ChunkProviderGenerate.class.getDeclaredField("gravelNoise");
            Field stoneNoiseF = ChunkProviderGenerate.class.getDeclaredField("stoneNoise");
            sandNoiseF.setAccessible(true);
            gravelNoiseF.setAccessible(true);
            stoneNoiseF.setAccessible(true);

            double[] sandNoise = (double[]) sandNoiseF.get(provider);
            double[] gravelNoise = (double[]) gravelNoiseF.get(provider);
            double[] stoneNoise = (double[]) stoneNoiseF.get(provider);

            // 3. Replicate the decision logic
            // Reset RNG to the state it would have at the start of replaceBlocksForBiome
            Random rand = new Random((long) 0 * 341873128712L + (long) 0 * 132897987541L);

            System.out.println("=== JAVA DECISIONS CHECK (Chunk 0,0) ===");

            // Check first 4 columns (x=0..3, z=0)
            for (int z = 0; z < 1; z++) {
                for (int x = 0; x < 4; x++) {
                    int idx = x + z * 16;

                    double sandRnd = rand.nextDouble();
                    boolean genSand = sandNoise[idx] + sandRnd * 0.2 > 0.0;

                    double gravelRnd = rand.nextDouble();
                    boolean genGravel = gravelNoise[idx] + gravelRnd * 0.2 > 3.0;

                    double stoneRnd = rand.nextDouble();
                    int stoneDepth = (int) (stoneNoise[idx] / 3.0 + 3.0 + stoneRnd * 0.25);

                    System.out.printf("Col (%d,%d): Sand[%b] Gravel[%b] Depth[%d]\n",
                            x, z, genSand, genGravel, stoneDepth);

                    // Consume the Bedrock RNG calls to keep sync for the next column
                    for (int y = 0; y < 128; y++)
                        rand.nextInt(5);

                    // Important: If we were simulating the full logic, we'd check for
                    // the sandstone nextInt(4) call here.
                    // But for now, let's just see if these 3 inputs match.
                }
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}