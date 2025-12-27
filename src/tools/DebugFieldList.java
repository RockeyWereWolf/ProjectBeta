import net.minecraft.src.ChunkProviderGenerate;
import java.lang.reflect.Field;

public class DebugFieldList {
    public static void main(String[] args) {
        System.out.println("=== Fields in ChunkProviderGenerate ===");
        try {
            for (Field f : ChunkProviderGenerate.class.getDeclaredFields()) {
                System.out.println(f.getName() + " (" + f.getType().getSimpleName() + ")");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}