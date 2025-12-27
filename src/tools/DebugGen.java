import java.util.Random;
// You need to copy the NoiseGenerator2 class from your decompiled source 
// into this file or reference it to compile this test.
// Assuming you can run this context:

public class DebugGen {
    public static void main(String[] args) {
        long biomeSeed = -987100L;
        Random rand = new Random(biomeSeed);

        // Initialize Simplex (Copy logic from WorldChunkManager)
        // This is NoiseGeneratorOctaves2, but we test just the first Octave
        // (NoiseGenerator2)

        // Emulate NoiseGenerator2 init
        double xo = rand.nextDouble() * 256.0D;
        double yo = rand.nextDouble() * 256.0D;
        double zo = rand.nextDouble() * 256.0D;

        int[] p = new int[512];
        for (int i = 0; i < 512; i++)
            p[i] = rand.nextInt(256);

        System.out.println("--- JAVA MATH CHECK ---");

        // Emulate func_4157_a (addNoise) for 0,0
        // We need to implement the math here to trace it, or call the class if
        // available.
        // Let's implement the math trace to be sure.

        double x = 0.0;
        double z = 0.0;
        double scaleX = 0.025;
        double scaleZ = 0.025;
        double amp = 0.25;

        double dx = x * scaleX + xo;
        double dz = z * scaleZ + yo;

        double F2 = 0.5 * (Math.sqrt(3.0) - 1.0);
        double G2 = (3.0 - Math.sqrt(3.0)) / 6.0;

        double s = (dx + dz) * F2;
        int i1 = (int) Math.floor(dx + s);
        int j1 = (int) Math.floor(dz + s);

        double t = (i1 + j1) * G2;
        double X0 = i1 - t;
        double Y0 = j1 - t;
        double x0 = dx - X0;
        double y0 = dz - Y0;

        int i2, j2;
        if (x0 > y0) {
            i2 = 1;
            j2 = 0;
        } else {
            i2 = 0;
            j2 = 1;
        }

        double x1 = x0 - i2 + G2;
        double y1 = y0 - j2 + G2;
        double x2 = x0 - 1.0 + 2.0 * G2;
        double y2 = y0 - 1.0 + 2.0 * G2;

        int ii = i1 & 255;
        int jj = j1 & 255;

        // Gradient Lookup
        int[][] grad3 = { { 1, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 }, { 1, 0, 1 }, { -1, 0, 1 },
                { 1, 0, -1 }, { -1, 0, -1 }, { 0, 1, 1 }, { 0, -1, 1 }, { 0, 1, -1 }, { 0, -1, -1 } };

        int gi0 = p[ii + p[jj]] % 12;
        int gi1 = p[ii + i2 + p[jj + j2]] % 12;
        int gi2 = p[ii + 1 + p[jj + 1]] % 12;

        double n0 = 0, n1 = 0, n2 = 0;

        double t0 = 0.5 - x0 * x0 - y0 * y0;
        if (t0 >= 0) {
            t0 *= t0;
            n0 = t0 * t0 * (grad3[gi0][0] * x0 + grad3[gi0][1] * y0);
        }

        double t1 = 0.5 - x1 * x1 - y1 * y1;
        if (t1 >= 0) {
            t1 *= t1;
            n1 = t1 * t1 * (grad3[gi1][0] * x1 + grad3[gi1][1] * y1);
        }

        double t2 = 0.5 - x2 * x2 - y2 * y2;
        if (t2 >= 0) {
            t2 *= t2;
            n2 = t2 * t2 * (grad3[gi2][0] * x2 + grad3[gi2][1] * y2);
        }

        double result = 70.0 * (n0 + n1 + n2) * amp;
        System.out.println("Noise Value at (0,0): " + result);
    }
}