#ifndef COLORINGS_H
#define COLORINGS_H



enum colorings {
    PROPER, ODD, pCFo, pCFc, iCFo, iCFc, pUMo, pUMc, iUMo, iUMc
};


// Colorings functions
int coloringIsProper(enum colorings col);
int coloringIsOpen(enum colorings col);
int coloringIsUM(enum colorings col);
int coloringIsCF(enum colorings col);
enum colorings getColoring(char coloring[]);



#endif