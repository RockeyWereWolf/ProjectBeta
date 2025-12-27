import java.util.Random;

public class DebugRNGRaw {
    // A helper class to expose the protected 'next' method from java.util.Random
    static class ParityRandom extends Random {
        public ParityRandom(long seed) {
            super(seed);
        }

        @Override
        protected int next(int bits) {
            return super.next(bits);
        }

        // This method performs only the integer part of nextDouble()
        public long getNextDoubleRawNumerator() {
            return ((long) (next(26)) << 27) + next(27);
        }
    }

    public static void main(String[] args) {
        // The seed for 'replaceBlocksForBiome' in chunk (0, 0) is 0L.
        long chunk_0_0_seed = 0L;
        ParityRandom random = new ParityRandom(chunk_0_0_seed);

        System.out.println("=== JAVA RNG GROUND TRUTH ===");

        // --- First Call ---
        long raw1 = random.getNextDoubleRawNumerator();
        System.out.println("\n--- Call 1 ---");
        System.out.println("Raw Numerator: " + raw1);
        System.out.printf("nextDouble():  %.17f\n", raw1 / (double) (1L << 53));

        // --- Second Call ---
        long raw2 = random.getNextDoubleRawNumerator();
        System.out.println("\n--- Call 2 ---");
        System.out.println("Raw Numerator: " + raw2);
        System.out.printf("nextDouble():  %.17f\n", raw2 / (double) (1L << 53));
    }
}