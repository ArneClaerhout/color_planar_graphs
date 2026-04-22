package graphs;

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
        add(ODD);
        add(pCFo);
        add(iCFo);
        add(pUMo);
        add(iUMo);
    }};

    private static final ArrayList<Coloring> conflictFreeList = new ArrayList<>(){{
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

    public static boolean isConflictFree(Coloring color) {
        return conflictFreeList.contains(color);
    }

    public static boolean isUniqueMaximum(Coloring color) {
        return uniqueMaximumList.contains(color);
    }

    public static Coloring getColoring(String coloring) {
        if (coloring == null) {
            // When there is no coloring given, use proper coloring
            return PROPER;
        }
        switch (coloring) {
            case "proper": return PROPER;
            case "odd": return ODD;
            case "pCFo": return pCFo;
            case "pCFc": return pCFc;
            case "iCFo": return iCFo;
            case "iCFc": return iCFc;
            case "pUMo": return pUMo;
            case "pUMc": return pUMc;
            case "iUMo": return iUMo;
            case "iUMc": return iUMc;
            default: return PROPER;
        }
    }
}


