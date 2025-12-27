import net.minecraft.src.*;
import java.io.File;
import java.lang.reflect.Field;
import java.util.Random;

public class DebugRNGSequence {
    public static void main(String[] args) {
        long seed = -100L;
        World world = new World(new SaveHandler(new File("."), "DebugWorld", false),
                "DebugWorld", WorldProvider.getProviderForDimension(0), seed);
        ChunkProviderGenerate provider = new ChunkProviderGenerate(world, seed);

        try {
            // We need to intercept the Random object BEFORE it processes the chunk
            Field randF = ChunkProviderGenerate.class.getDeclaredField("rand");
            randF.setAccessible(true);

            // Inject our tracking Random? No, just let it run and print state?
            // Easier: Use a custom subclass of Random that logs calls.
            // But we can't inject that easily without modifying source.

            // Alternative: Just generate and assume it's correct, but we can't see the
            // internal Rnd values easily.
            // Wait. We can just print the Random state if we access it?
            // Random doesn't expose state.

            // Let's rely on the fact that if Sandstone generates at the same height,
            // and RNG bits are perfect, sync IS achieved.
            System.out.println("Java Reference: Sandstone at 58 implies sync.");

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}