import net.minecraft.src.*;
import java.io.File;
import java.util.Random;

public class DebugRNG {
    public static void main(String[] args) {
        long seed = -100L;
        int chunkX = 0;
        int chunkZ = 0;

        System.out.println("=== JAVA RNG PARITY CHECK (Chunk 0,0) ===");

        // We only need the Random object for this test.
        // The seed logic is taken directly from ChunkProviderGenerate.java
        Random random = new Random();
        random.setSeed((long) chunkX * 341873128712L + (long) chunkZ * 132897987541L);

        // This simulates the RNG consumption for the FIRST column (x=0, z=0)
        // inside replaceBlocksForBiome.
        System.out.println("--- Column (0,0) ---");

        // 1. Sand check
        double sandRNG = random.nextDouble();
        System.out.printf("1. Sand Check (nextDouble):      %.17f\n", sandRNG);

        // 2. Gravel check
        double gravelRNG = random.nextDouble();
        System.out.printf("2. Gravel Check (nextDouble):    %.17f\n", gravelRNG);

        // 3. Stone Depth check
        double stoneRNG = random.nextDouble();
        System.out.printf("3. Stone Depth (nextDouble):     %.17f\n", stoneRNG);

        // 4. Bedrock check for y=5
        int bedrockRNG_y5 = random.nextInt(5);
        System.out.printf("4. Bedrock y=5 (nextInt(5)):     %d\n", bedrockRNG_y5);

        // 5. Bedrock check for y=4
        int bedrockRNG_y4 = random.nextInt(5);
        System.out.printf("5. Bedrock y=4 (nextInt(5)):     %d\n", bedrockRNG_y4);

        // 6. Bedrock check for y=3
        int bedrockRNG_y3 = random.nextInt(5);
        System.out.printf("6. Bedrock y=3 (nextInt(5)):     %d\n", bedrockRNG_y3);

        // 7. Bedrock check for y=2
        int bedrockRNG_y2 = random.nextInt(5);
        System.out.printf("7. Bedrock y=2 (nextInt(5)):     %d\n", bedrockRNG_y2);

        // 8. Bedrock check for y=1
        int bedrockRNG_y1 = random.nextInt(5);
        System.out.printf("8. Bedrock y=1 (nextInt(5)):     %d\n", bedrockRNG_y1);
    }
}