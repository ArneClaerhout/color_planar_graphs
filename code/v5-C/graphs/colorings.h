#ifndef COLORINGS_H
#define COLORINGS_H



enum colorings {
    PROPER, ODD, pCFo, pCFc, iCFo, iCFc, pUMo, pUMc, iUMo, iUMc
};


// Colorings functions
/**
 * Finds whether a coloring is proper.
 *
 * @param col The coloring.
 * @return One, if the coloring is proper.
 *         Zero, otherwise.
 */
int coloringIsProper(enum colorings col);

/**
 * Finds whether a coloring is using open neighborhoods.
 *
 * @param col The coloring.
 * @return One, if the coloring is using open neighborhoods.
 *         Zero, otherwise.
 */
int coloringIsOpen(enum colorings col);

/**
 * Finds whether a coloring is a unique-maximum coloring.
 *
 * @param col The coloring.
 * @return One, if the coloring is unique-maximum.
 *         Zero, otherwise.
 */
int coloringIsUM(enum colorings col);

/**
 * Finds whether a coloring is a conflict-free coloring.
 *
 * @param col The coloring.
 * @return One, if the coloring is conflict-free.
 *         Zero, otherwise.
 */
int coloringIsCF(enum colorings col);

/**
 * Finds the respective coloring of a given string.
 *
 * @param coloring The coloring in string format.
 * @return The coloring in enum format.
 */
enum colorings getColoring(char coloring[]);



#endif