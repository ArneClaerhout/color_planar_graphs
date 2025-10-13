import java.util.ArrayList;

public enum Coloring {
    PROPER(4),
    ODD(8),
    pCFo(8),
    pCFc(4),
    iCFo(5),
    iCFc(4),
    pUMo(10),
    pUMc(8),
    iUMo(10),
    iUMc(6);

    private final int maxChromaticNumber;

    private static final ArrayList<Coloring> properList = new ArrayList<>(){{
        add(PROPER);
        add(ODD);
        add(pCFo);
        add(pCFc);
        add(pUMo);
        add(pUMc);
    }};

    private static final ArrayList<Coloring> openList = new ArrayList<>(){{
        add(PROPER);
        add(ODD);
        add(pCFo);
        add(iCFo);
        add(pUMo);
        add(iUMo);
    }};

    private static final ArrayList<Coloring> contextFreeList = new ArrayList<>(){{
        add(pCFo);
        add(pCFc);
        add(iCFo);
        add(iCFc);
    }};

    private static final ArrayList<Coloring> uniqueMaximumList = new ArrayList<>(){{
        add(pUMo);
        add(pUMc);
        add(iUMo);
        add(iUMc);
    }};

    Coloring(int i) {
        maxChromaticNumber = i;
    }

    public int getMaxChromaticNumber() {
        return maxChromaticNumber;
    }

    public static boolean isProper(Coloring color) {
        return properList.contains(color);
    }

    public static boolean isOpen(Coloring color) {
        return openList.contains(color);
    }

    public static boolean isContextFree(Coloring color) {
        return contextFreeList.contains(color);
    }

    public static boolean isUniqueMaximum(Coloring color) {
        return uniqueMaximumList.contains(color);
    }
}


