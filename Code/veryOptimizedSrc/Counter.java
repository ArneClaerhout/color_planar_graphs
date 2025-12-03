import java.util.ArrayList;

public class Counter {

    private int condition = 0;

    private final ArrayList<int[]> colorings = new ArrayList<>();

    public Counter() {}

    public void inputColors(int[] colors, boolean allColors) {
        for (int k = 0; k < colors.length; k++) {
            if (colors[k] == 1) condition |= (1 << k);
        }
        if(allColors) colorings.add(colors);
    }

    public int getCondition() {
        return condition;
    }

    public ArrayList<int[]> getColorings() {
        return colorings;
    }
}
