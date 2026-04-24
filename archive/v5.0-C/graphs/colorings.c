#include "colorings.h"
#include <string.h>

int coloringIsProper(enum colorings col) {
    return col < 4 || col == 6 || col == 7;
}

int coloringIsOpen(enum colorings col) {
    return (col % 2 == 0 && col) || col == 1;
}

int coloringIsUM(enum colorings col) {return col > 5;
}

int coloringIsCF(enum colorings col) {
    return col > 1 && col < 6;
}

enum colorings getColoring(char coloring[]) {
    if (coloring == NULL) {
        return PROPER;
    }

    if (strcmp(coloring, "proper") == 0) return PROPER;
    if (strcmp(coloring, "odd") == 0)    return ODD;
    if (strcmp(coloring, "pCFo") == 0)   return pCFo;
    if (strcmp(coloring, "pCFc") == 0)   return pCFc;
    if (strcmp(coloring, "iCFo") == 0)   return iCFo;
    if (strcmp(coloring, "iCFc") == 0)   return iCFc;
    if (strcmp(coloring, "pUMo") == 0)   return pUMo;
    if (strcmp(coloring, "pUMc") == 0)   return pUMc;
    if (strcmp(coloring, "iUMo") == 0)   return iUMo;
    if (strcmp(coloring, "iUMc") == 0)   return iUMc;

    return PROPER;
}