package graphs;

import java.util.ArrayList;
import java.util.Arrays;

public class ColoringCounter {

    private long[] condition = new long[10];

    private int colorToCheck;

    private long maxColoring;

    private final ArrayList<int[]> colorings = new ArrayList<>();

    public ColoringCounter() {
        this.colorToCheck = 1;
    }

    public ColoringCounter(int colorToCheck) {
        this.colorToCheck = colorToCheck;
    }

    public void setColorToCheck(int colorToCheck) {
        if (!colorings.isEmpty() || !Arrays.stream(condition).allMatch(n -> n == 0))
            throw new IllegalStateException("Already started trying colorings, can't change colorToCheck.");
        this.colorToCheck = colorToCheck;
    }

    public boolean inputColors(int[] colors, boolean allColors) {
        if (maxColoring == 0) {
            maxColoring = (1L << (colors.length + 1)) - 1;
        }
        for (int k = 0; k < colors.length; k++) {
            for (int i = 0; i < Arrays.stream(colors).max().getAsInt(); i++) {
                if (colors[k] == i) {
                    condition[i - 1] |= (1L << k);
                }
            }

        }
        if (allColors) {
            colorings.add(colors);
        } else {
            // We check if all of them are full
            if (!Arrays.stream(condition).allMatch(n -> n == 0) && Arrays.stream(condition).allMatch(n -> (n == 0 || n == maxColoring))) {
                return false;
            }
        }
        return true;
    }

    public long getCondition(int index) {
        return condition[index];
    }

    public long[] getCondition() {
        return condition;
    }

    public ArrayList<int[]> getColorings() {
        return colorings;
    }
}
