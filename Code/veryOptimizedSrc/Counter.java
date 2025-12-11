import java.util.ArrayList;
import java.util.Arrays;

public class Counter {

    private long condition = 0;

    private final int maxColor;

    private final ArrayList<int[]> colorings = new ArrayList<>();

    public Counter() {
        this.maxColor = 3;
    }

    public Counter(int maxColor) {
        this.maxColor = maxColor;
    }

    public void inputColors(int[] colors, boolean allColors) {
        for (int k = 0; k < colors.length; k++) {
            if (colors[k] == 3) {
                condition |= (1L << k);
            }
        }
        if(allColors) colorings.add(colors);
    }

    public long getCondition() {
        return condition;
    }

    public ArrayList<int[]> getColorings() {
        return colorings;
    }
}
