import java.util.Random;

public class DebugCPPNextDouble {

    // A custom Random class that lets us access the protected 'next' method
    static class ParityRandom extends Random {
        public ParityRandom(long seed) {
            super(seed);
        }

        @Override
        protected int next(int bits) {
            return super.next(bits);
        }

        // This method mimics the slightly incorrect result from your C++ environment
        // by adding 1 to the 53-bit integer numerator before division.
        // This simulates the 1 Unit in the Last Place (ULP) error we observed.
        public double nextDoubleCpp() {
            long l = ((long) (next(26)) << 27) + next(27);
            return (l + 1.0) / (double) (1L << 53);
        }
    }

    public static void main(String[] args) {
        // The seed for the 'replaceBlocksForBiome' function is NOT the world seed.
        // It is calculated from the chunk coordinates. For chunk (0, 0), this seed is
        // 0.
        long chunk_0_0_seed = 0L;

        // Use our custom Random class, now correctly initialized with the seed.
        ParityRandom random = new ParityRandom(chunk_0_0_seed);

        System.out.println("=== JAVA SIMULATING C++ RNG (Chunk 0,0 Seed) ===");
        System.out.println("--- Column (0,0) ---");

        System.out.printf("1. Sand Check (nextDoubleCpp):   %.17f\n", random.nextDoubleCpp());
        System.out.printf("2. Gravel Check (nextDoubleCpp): %.17f\n", random.nextDoubleCpp());
        System.out.printf("3. Stone Depth (nextDoubleCpp):  %.17f\n", random.nextDoubleCpp());
        System.out.printf("4. Bedrock y=5 (nextInt(5)):     %d\n", random.nextInt(5));
        System.out.printf("5. Bedrock y=4 (nextInt(5)):     %d\n", random.nextInt(5));
        System.out.printf("6. Bedrock y=3 (nextInt(5)):     %d\n", random.nextInt(5));
        System.out.printf("7. Bedrock y=2 (nextInt(5)):     %d\n", random.nextInt(5));
        System.out.printf("8. Bedrock y=1 (nextInt(5)):     %d\n", random.nextInt(5));
    }
}