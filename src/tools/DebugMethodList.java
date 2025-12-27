import net.minecraft.src.NoiseGeneratorPerlin;
import java.lang.reflect.Method;

public class DebugMethodList {
    public static void main(String[] args) {
        System.out.println("=== Methods in NoiseGeneratorPerlin ===");
        try {
            for (Method m : NoiseGeneratorPerlin.class.getDeclaredMethods()) {
                System.out.print(m.getName() + "(");
                Class<?>[] params = m.getParameterTypes();
                for (int i = 0; i < params.length; i++) {
                    System.out.print(params[i].getSimpleName());
                    if (i < params.length - 1)
                        System.out.print(", ");
                }
                System.out.println(")");
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}