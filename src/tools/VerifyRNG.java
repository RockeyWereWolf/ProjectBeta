import java.util.Random;

public class VerifyRNG {
    public static void main(String[] args) {
        long seed = 0L; // Chunk 0,0 derived seed
        Random r = new Random(seed);

        System.out.println("=== Java Chunk 0,0 Column 0 Simulation ===");

        // 1. Surface Noise Randoms
        r.nextDouble(); // Sand
        r.nextDouble(); // Gravel
        r.nextDouble(); // Stone

        // 2. Bedrock Loop (127 down to 0)
        for (int y = 127; y >= 0; y--) {
            int bedrockRandom = r.nextInt(5);

            // We want to know what happens at Y=1
            if (y == 1) {
                System.out.println("Y=1: nextInt(5) = " + bedrockRandom);
                System.out.println("     Bedrock check (y <= rand): " + (y <= bedrockRandom));
            }
        }
    }
}