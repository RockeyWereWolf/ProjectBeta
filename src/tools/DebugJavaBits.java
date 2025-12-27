import java.util.Random;

public class DebugJavaBits {
    // Custom Random to access protected next()
    static class ParityRandom extends Random {
        public ParityRandom(long seed) {
            super(seed);
        }

        @Override
        protected int next(int bits) {
            return super.next(bits);
        }

        public double nextDoubleRaw() {
            // Helper to see what the inputs are
            long l = ((long) (next(26)) << 27) + next(27);
            return l / (double) (1L << 53);
        }
    }

    public static void main(String[] args) {
        ParityRandom random = new ParityRandom(0L); // Seed 0
        System.out.println("=== JAVA RAW BITS CHECK ===");

        // Call 1
        double val1 = random.nextDoubleRaw();
        long bits1 = Double.doubleToLongBits(val1);
        System.out.printf("Call 1 Value: %.20f\n", val1);
        System.out.printf("Call 1 Bits:  0x%016X\n", bits1);
    }
}