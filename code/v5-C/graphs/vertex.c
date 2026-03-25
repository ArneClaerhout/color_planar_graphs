#include "colorings.h"
#include "vertex.h"
#include "types.h"
#include "graph.h"
#include <stdint.h>
#include <stdio.h>

void addNeighbour(vertex* v, vertex* neighbour) {
    v->neighbours |= SHIFTL(neighbour->index);
    neighbour->neighbours |= SHIFTL(v->index);
}

void removeNeighbour(vertex* v, vertex* neighbour) {
    v->neighbours &= ~(SHIFTL(neighbour->index));
    neighbour->neighbours &= ~(SHIFTL(v->index));
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
    bitset_t neighbourhood = isOpenColoring ? v->neighbours : (v->neighbours | SHIFTL(v->index));

    int odds = 0;
    FOR_EACH_BIT(index, neighbourhood) {
        odds ^= SHIFT(verticesIndexed[index].color);
    }
    return odds != 0;
}

int isCorrectlyColoredProper(vertex* v, vertex verticesIndexed[]) {
    return 1;
}

int isCorrectlyColoredUM(vertex* v, vertex verticesIndexed[]) {
    bitset_t neighbourhood = isOpenColoring ? v->neighbours : (v->neighbours | SHIFTL(v->index));
    int max = 0;
    int amountOfMax = 0;
    FOR_EACH_BIT(index, neighbourhood) {
        int neighbourColor = verticesIndexed[index].color;

        if (neighbourColor > max) {
            max = neighbourColor;
            amountOfMax = 1;
        } else if (neighbourColor == max) {
            amountOfMax++;
        }
    }
    return amountOfMax == 1;
}

int isCorrectlyColoredCF(vertex* v, vertex verticesIndexed[]) {
    bitset_t neighbourhood = isOpenColoring ? v->neighbours : (v->neighbours | SHIFTL(v->index));

    int colorsOccurOnce = 0;
    int colorsOccur = 0;

    FOR_EACH_BIT(index, neighbourhood) {
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

    bitset_t neighbourhood = isOpenColoring ? v->neighbours : (v->neighbours | SHIFTL(v->index));

    if (coloring == ODD) {
        int odds = 0;
        FOR_EACH_BIT(index, neighbourhood) {
            vertex* neighbour = verticesIndexed[index];
            int neighbourColor = neighbour->color;

            if (neighbourColor == 0) {
                // fillUncolored is true
                if (neighbour->amountOfAvailableColors == 1) {
                    odds ^= SHIFT(__builtin_ctz(neighbour->availableColors));
                } else {
                    return 0;
                }
            } else {
                odds ^= SHIFT(neighbourColor);
            }
            if (neighbourColor == v->color && v != neighbour) return 0;
        }
        return odds != 0;
    } else if (coloring == PROPER) {
        return 1;
    } else if (isUMColoring) {
        // Unique-Maximum
        int max = 0;
        int amountOfMax = 0;
        FOR_EACH_BIT(index, neighbourhood) {
            vertex* neighbour = verticesIndexed[index];
            int neighbourColor = neighbour->color;

            if (neighbourColor == 0) {
                // fillUncolored is true, we check if we can fill the uncolored vertex.
                if (neighbour->amountOfAvailableColors == 1) {
                    neighbourColor = __builtin_ctz(neighbour->availableColors);
                } else {
                    return 0;
                }
            }

            if (isProperColoring && neighbourColor == v->color && v != neighbour) {
                return 0;
            }

            if (neighbourColor > max) {
                max = neighbourColor;
                amountOfMax = 1;
            } else if (neighbourColor == max) {
                amountOfMax++;
            }
        }
        return amountOfMax == 1;
    } else {
        // Conflict-free
        int colorsOccurOnce = 0;
        int colorsOccur = 0;
        int colorIndex;

        FOR_EACH_BIT(index, neighbourhood) {
            vertex* neighbour = verticesIndexed[index];
            int neighbourColor = neighbour->color;
            // We do -1 as the colors are from 1...k,
            // but we want to later on use the colors 0...k-1

            if (neighbourColor == 0) {
                // fillUncolored is true, we check if we can fill the uncolored vertex.
                if (neighbour->amountOfAvailableColors == 1) {
                    colorIndex = SHIFT(__builtin_ctz(neighbour->availableColors));
                } else {
                    return 0;
                }
            } else {
                colorIndex = SHIFT((neighbourColor - 1));
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
