#include "colorings.h"
#include "vertex.h"
#include "types.h"
#include <stdint.h>
#include <stdio.h>

extern int isOpenColoring;
extern int isUMColoring;
extern int isProperColoring;

void addNeighbor(vertex* v, vertex* neighbor) {
    v->neighbors |= SHIFTL(neighbor->index);
    neighbor->neighbors |= SHIFTL(v->index);
}

void removeNeighbor(vertex* v, vertex* neighbor) {
    v->neighbors &= ~(SHIFTL(neighbor->index));
    neighbor->neighbors &= ~(SHIFTL(v->index));
}

void setMaxAvailableColors(vertex* v, int max) {
    v->availableColors = SHIFT(max) - 1;
    v->amountOfAvailableColors = max;
}

// int removeColorFromAvailableColors(vertex* v, int color) {
//     if (v->availableColors & SHIFT(color)) {
//         v->availableColors &= ~(SHIFT(color));
//         v->amountOfAvailableColors--;
//         return 1;
//     }
//     return 0;
// }

// int addColorFromAvailableColors(vertex* v, int color) {
//     if (!(v->availableColors & SHIFT(color))) {
//         v->availableColors |= SHIFT(color);
//         v->amountOfAvailableColors++;
//         return 1;
//     }
//     return 0;
// }

int isCorrectlyColoredOdd(vertex* v, vertex verticesIndexed[]) {
    bitset_t neighborhood = isOpenColoring ? v->neighbors : (v->neighbors | SHIFTL(v->index));

    int odds = 0;
    FOR_EACH_BIT(index, neighborhood) {
        odds ^= SHIFT(verticesIndexed[index].color);
    }
    return odds != 0;
}

int isCorrectlyColoredProper(vertex* v, vertex verticesIndexed[]) {
    return 1;
}

int isCorrectlyColoredUM(vertex* v, vertex verticesIndexed[]) {
    bitset_t neighborhood = isOpenColoring ? v->neighbors : (v->neighbors | SHIFTL(v->index));
    int max = 0;
    int amountOfMax = 0;
    FOR_EACH_BIT(index, neighborhood) {
        int neighborColor = verticesIndexed[index].color;

        if (neighborColor > max) {
            max = neighborColor;
            amountOfMax = 1;
        } else if (neighborColor == max) {
            amountOfMax++;
        }
    }
    return amountOfMax == 1;
}

int isCorrectlyColoredCF(vertex* v, vertex verticesIndexed[]) {
    bitset_t neighborhood = isOpenColoring ? v->neighbors : (v->neighbors | SHIFTL(v->index));

    int colorsOccurOnce = 0;
    int colorsOccur = 0;

    FOR_EACH_BIT(index, neighborhood) {
        int colorIndex = SHIFT((verticesIndexed[index].color - 1));
        // We do -1 as the colors are from 1...k,
        // but we want to later on use the colors 0...k-1

        if ((colorsOccur & colorIndex) != 0) {
            colorsOccurOnce &= ~colorIndex;
        } else {
            colorsOccurOnce |= colorIndex;
            colorsOccur |= colorIndex;
        }
    }
    return colorsOccurOnce != 0;
}


int isCorrectlyColored(vertex* v, vertex* verticesIndexed[], enum colorings coloring) {
    if (v->color == 0 && coloringIsOpen(coloring) == 0) return 0;

    bitset_t neighborhood = isOpenColoring ? v->neighbors : (v->neighbors | SHIFTL(v->index));

    if (coloring == ODD) {
        int odds = 0;
        FOR_EACH_BIT(index, neighborhood) {
            vertex* neighbor = verticesIndexed[index];
            int neighborColor = neighbor->color;

            if (neighborColor == 0) {
                // fillUncolored is true
                if (neighbor->amountOfAvailableColors == 1) {
                    odds ^= SHIFT(__builtin_ctz(neighbor->availableColors));
                } else {
                    return 0;
                }
            } else {
                odds ^= SHIFT(neighborColor);
            }
            if (neighborColor == v->color && v != neighbor) return 0;
        }
        return odds != 0;
    } else if (coloring == PROPER) {
        return 1;
    } else if (isUMColoring) {
        // Unique-Maximum
        int max = 0;
        int amountOfMax = 0;
        FOR_EACH_BIT(index, neighborhood) {
            vertex* neighbor = verticesIndexed[index];
            int neighborColor = neighbor->color;

            if (neighborColor == 0) {
                // fillUncolored is true, we check if we can fill the uncolored vertex.
                if (neighbor->amountOfAvailableColors == 1) {
                    neighborColor = __builtin_ctz(neighbor->availableColors);
                } else {
                    return 0;
                }
            }

            if (isProperColoring && neighborColor == v->color && v != neighbor) {
                return 0;
            }

            if (neighborColor > max) {
                max = neighborColor;
                amountOfMax = 1;
            } else if (neighborColor == max) {
                amountOfMax++;
            }
        }
        return amountOfMax == 1;
    } else {
        // Conflict-free
        int colorsOccurOnce = 0;
        int colorsOccur = 0;
        int colorIndex;

        FOR_EACH_BIT(index, neighborhood) {
            vertex* neighbor = verticesIndexed[index];
            int neighborColor = neighbor->color;
            // We do -1 as the colors are from 1...k,
            // but we want to later on use the colors 0...k-1

            if (neighborColor == 0) {
                // fillUncolored is true, we check if we can fill the uncolored vertex.
                if (neighbor->amountOfAvailableColors == 1) {
                    colorIndex = SHIFT(__builtin_ctz(neighbor->availableColors));
                } else {
                    return 0;
                }
            } else {
                colorIndex = SHIFT((neighborColor - 1));
            }

            if ((colorsOccur & colorIndex) != 0) {
                colorsOccurOnce &= ~colorIndex;
            } else {
                colorsOccurOnce |= colorIndex;
                colorsOccur |= colorIndex;
            }
        }
        return colorsOccurOnce != 0;
    }
    return 0;
}
