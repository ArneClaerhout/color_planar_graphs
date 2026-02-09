package graphs;

import java.util.ArrayList;
import java.util.Arrays;

public class ColoringCounter {

    private long[] condition;

    private long conditionVertices;

    private final int numberOfVertices;

    private final long maxColoring;

    private boolean checkAlwaysColor;

    private final ArrayList<int[]> colorings = new ArrayList<>();

    public ColoringCounter(long maxColoring) {
        this.maxColoring = maxColoring;
        conditionVertices = maxColoring;
        numberOfVertices = 64 - Long.numberOfLeadingZeros(maxColoring);
        checkAlwaysColor = false;
    }

    public ColoringCounter(long maxColoring, boolean checkAlwaysColor) {
        this.maxColoring = maxColoring;
        conditionVertices = maxColoring;
        numberOfVertices = 64 - Long.numberOfLeadingZeros(maxColoring);
        this.checkAlwaysColor = checkAlwaysColor;
    }

    public boolean inputColors(int[] colors, boolean allColors, int chromaticNumber) {
        long oldCondition = 0;
        if (condition == null) {
            condition = new long[chromaticNumber];
        }
        for (int k = 0; k < colors.length; k++) {
            // Here we add
            if (checkAlwaysColor) {
                oldCondition = condition[colors[k] - 1];
                condition[colors[k] - 1] |= (1L << k);
                if (oldCondition != 0 && oldCondition != condition[colors[k] - 1]) {
                    // The vertex already had a color, and it wasn't this one
                    conditionVertices &= ~(1L << k);
                }
            } else {
                // When checking if a vertex is never a color, the above process would be too time-consuming
                condition[colors[k] - 1] |= (1L << k);
            }

        }
        if (allColors) {
            colorings.add(colors);
        } else {
            // We check if all of them are full, if they are: stop
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

    public void setCheckAlwaysColor(boolean checkAlwaysColor) {
        this.checkAlwaysColor = checkAlwaysColor;
    }

    public boolean isConditionMet() {
        return (checkAlwaysColor && conditionVertices != 0) || (!checkAlwaysColor && !Arrays.stream(condition).allMatch(n -> (n == maxColoring)));
    }

    public int[] getColoringAfterCheck() {
        if (!isConditionMet()) {
            throw new IllegalStateException("Requested coloring after check when condition isn't met.");
        }
        int[] colors = new int[numberOfVertices];
        if (checkAlwaysColor) {
            for (long k = conditionVertices; k != 0; k &= k - 1) {
                int index = Long.numberOfTrailingZeros(k);
                for (int i = 0; i < condition.length; i++) {
                    if ((condition[i] & (1L << index)) != 0) {

                        // It always has the color i + 1
                        colors[index] = i + 1;
                        break;
                    }
                }
            }
        } else {
            for (int i = 0; i < condition.length; i++) {
                if (getCondition(i) != maxColoring) {
                    for (long k = (maxColoring & ~getCondition(i)); k != 0; k &= k - 1) {
                        int index = Long.numberOfTrailingZeros(k);
                        colors[index] = i + 1;
                    }
                    break;
                }
            }
        }
        return colors;
    }
}
