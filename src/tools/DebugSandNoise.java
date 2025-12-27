
import java.util.Random;
import net.minecraft.src.NoiseGeneratorOctaves;

public class DebugSandNoise {
    public static void main(String[] args) {
        long seed = 12345L;
        // In ChunkProviderGenerate.replaceBlocksForBiome:
        // double scale = 0.03125D;
        // sandNoise = field_909_n.generateNoiseOctaves(sandNoise, chunkX*16, chunkZ*16,
        // 0.0D, 16, 16, 1, scale, scale, 1.0D);

        // Emulate Initialization
        Random rand = new Random(seed);
        NoiseGeneratorOctaves field_912_k = new NoiseGeneratorOctaves(rand, 16);
        NoiseGeneratorOctaves field_911_l = new NoiseGeneratorOctaves(rand, 16);
        NoiseGeneratorOctaves field_910_m = new NoiseGeneratorOctaves(rand, 8);
        NoiseGeneratorOctaves field_909_n = new NoiseGeneratorOctaves(rand, 4); // Used for Sand/Gravel

        // We only care about sandNoise for now as the pattern is representative
        double[] sandNoise = new double[256];
        double scale = 0.03125D;

        int chunkX = 0;
        int chunkZ = 0;

        // NOTE: The arguments in Java are (array, x, y, z, sizeX, sizeY, sizeZ, scaleX,
        // scaleY, scaleZ)
        // Implementation: generateNoiseOctaves(this.sandNoise, (double)(var1 * 16),
        // (double)(var2 * 16), 0.0D, 16, 16, 1, var6, var6, 1.0D);
        // var1 = chunkX, var2 = chunkZ
        // x=chunkX*16, y=chunkZ*16, z=0
        // sizeX=16, sizeY=16, sizeZ=1
        // scaleX=scale, scaleY=scale, scaleZ=1.0

        // WAIT! The Java args are tricky.
        // func_805_a(double[] var1, double var2, double var4, double var6, int var8,
        // int var9, int var10, double var11, double var13, double var15, double var17)
        // var2=x, var4=y, var6=z.
        // BUT NoiseGeneratorPerlin's noise is 3D.
        // Let's look closely at: this.sandNoise =
        // this.field_909_n.generateNoiseOctaves(..., (double)(var1 * 16), (double)(var2
        // * 16), 0.0D, 16, 16, 1, var6, var6, 1.0D);
        // X = chunkX*16
        // Y = chunkZ*16 !! (In Java 3D noise usually Y is up, but here they map Z world
        // coord to Y noise coord?)
        // Z = 0
        // sizeX = 16
        // sizeY = 16
        // sizeZ = 1

        sandNoise = field_909_n.generateNoiseOctaves(sandNoise, (double) (chunkX * 16), (double) (chunkZ * 16), 0.0D,
                16, 16, 1, scale, scale, 1.0D);

        System.out.println("=== JAVA SAND NOISE (Chunk 0,0) ===");
        System.out.println("Col 0 (x=0, z=0..15):");
        for (int i = 0; i < 16; i++) {
            // In replaceBlocksForBiome:
            // double sandVal = this.sandNoise[var8 + var9 * 16] + this.rand.nextDouble() *
            // 0.2D;
            // var8 is x (0-15), var9 is z (0-15).
            // Index is x + z*16 ??
            // "var10 = var4[var8 + var9 * 16];" -> Biome array uses this index.
            // "this.sandNoise[var8 + var9 * 16]" -> Sand noise uses this index.

            // Wait, let's check the NoiseGeneratorOctaves...
            // It fills loops: for(x) { for(z) { for(y) ... } } or similar?
            // Actually NoiseGeneratorPerlin.func_805_a (which Octaves calls)
            // Loops: if (var9==1) // sizeY==1 (Our case: sizeZ=1 passed as last dim from
            // Octaves?)

            // Wait, generateNoiseOctaves calls Perlin.func_805_a arguments:
            // (array, x, y, z, sizeX, sizeY, sizeZ, scaleX, scaleY, scaleZ, amp)
            // passed: (array, x, y, z, sizeX, sizeY, sizeZ, scaleX, scaleY, scaleZ)

            // Java invocation: generateNoiseOctaves(..., 16, 16, 1, ...)
            // xSize=16, ySize=16, zSize=1.

            // Inside Perlin.func_805_a:
            // if (var9 == 1) // ySize == 1 ? No, var9 is ySize.
            // Here ySize is 16. So it goes to else block.

            // Loops in else block:
            // for(x < sizeX) {
            // for(z < sizeZ) {
            // for(y < sizeY) {
            // idx++
            // }
            // }
            // }

            // Order: X, Z, Y.
            // x goes 0..15
            // z goes 0..0 (sizeZ=1)
            // y goes 0..15 (sizeY=16)

            // So the array layout is:
            // Index = ((x * sizeZ) + z) * sizeY + y
            // Index = ((x * 1) + 0) * 16 + y
            // Index = x*16 + y

            // In replaceBlocksForBiome:
            // val = sandNoise[x + z*16];
            // Here x is var8, z is var9.
            // So index is x + z*16.
            // Wait.
            // If array layout is x*16 + y...
            // and we access x + z*16...
            // That means 'y' in noise gen corresponds to 'z' in world loop??

            // Let's verify the Java arguments again.
            // generateNoiseOctaves(..., chunkX*16, chunkZ*16, 0.0, 16, 16, 1, ...)
            // x=chunkX*16
            // y=chunkZ*16
            // z=0
            // sizeX=16
            // sizeY=16
            // sizeZ=1

            // So 'X' dim corresponds to chunkX (World X)
            // 'Y' dim corresponds to chunkZ (World Z)
            // 'Z' dim is 1.

            // Array layout (X, Z, Y loop order in Perlin?):
            // Actually, looking at NoiseGeneratorPerlin.java:
            // loop 1: var37 < var8 (xSize)
            // loop 2: var44 < var10 (zSize)
            // loop 3: var51 < var9 (ySize)
            // index++

            // So buffer[ ((x * zSize) + z) * ySize + y ]

            // Here:
            // xSize = 16
            // ySize = 16
            // zSize = 1

            // buffer [ ((x * 1) + 0) * 16 + y ] = buffer[ x*16 + y ]

            // In replaceBlocks:
            // val = buffer[ x + z*16 ]

            // If x=0..15 corresponding to sizeX
            // and z=0..15 corresponding to sizeY
            // Then buffer[ x*16 + z ] would mean x=Major, z=Minor.

            // But replaceBlocks accesses [ x + z*16 ].
            // That implies z is Major (stride 16), x is Minor.

            // Mismatch?!
            // If buffer is [ x*16 + y ] (where y is world Z)
            // Then buffer[0] is x=0, y=0
            // buffer[1] is x=0, y=1
            // buffer[16] is x=1, y=0

            // replaceBlocks at x=0, z=1:
            // access [0 + 1*16] = [16]
            // Retrieves x=1, y=0 from noise?
            // But we wanted x=0, z=1 (which is y=1 in noise)

            // So we wanted buffer[1] (x=0, y=1)
            // But we accessed buffer[16] (x=1, y=0)

            // CONCLUSION: Effectively Transposed?
            // If we wanted Noise(0,1) and got Noise(1,0).
            // Since sand noise is 2D fractal, swapping X/Y coords = Transpose.
        }

        // Let's print the first few values to be absolutely sure what Java produces.
        // We will print: Index, Value.
        for (int i = 0; i < 16; i++) {
            System.out.printf("Index %d: %.10f%n", i, sandNoise[i]);
        }
        System.out.println("...");
        for (int i = 0; i < 4; i++) {
            int idx = 16 + i;
            System.out.printf("Index %d: %.10f%n", idx, sandNoise[idx]);
        }
    }
}
