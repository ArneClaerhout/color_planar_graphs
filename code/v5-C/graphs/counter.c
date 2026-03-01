#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include "vertex.h"
#include "graph.h"

extern int um;
extern int proper;
extern int open;
extern enum colorings coloring;
extern graph* g;

int inputColors(counter* counter, const int colors[], int allColors, int chromaticNumber) {
    if (um) {
        uint64_t oldCondition = 0;
        for (int k = 0; k < counter->numberOfVertices; k++) {
            // Here we add
            oldCondition = counter->condition[colors[k] - 1];
            counter->condition[colors[k] - 1] |= SHIFTL(k);
            if (!counter->firstInput && oldCondition != counter->condition[colors[k] - 1]) {
                // The vertex already had a color, and it wasn't this one
                counter->conditionVertices &= ~SHIFTL(k);
            }
        }
        if (counter->firstInput) counter->firstInput = 0;
        if (allColors) {
            fprintf(stderr, "Displaying all colorings is currently not implemented.");
            exit(1);
        } else {
            // We check if all of them are full, if they are: stop
            for (int i = 0; i < chromaticNumber; i++) {
                if (counter->condition[i] != counter->maxColoringMask) {
                    return 0;
                }
            }
            return 1;
        }
    } else if (coloring == PROPER || coloring == ODD) {
        return 0;
    } else if (proper && open) {
        // This is CF coloring (only useful for pCFo)
        for (int k = 0; k < counter->numberOfVertices; k++) {
            int colorsForVertex = 0;
            FOR_EACH_BIT(index, g->verticesIndexed[k].neighbours) {
                colorsForVertex |= SHIFT(g->verticesIndexed[index].color);
            }
            if (__builtin_popcount(colorsForVertex) != chromaticNumber - 1) {
                counter->conditionVertices &= ~SHIFTL(k);
            }
        }
        if (counter->conditionVertices == 0) {
            return 1;
        }
        return 0;
    } else {
        return 1;
    }

}

int isConditionMet(counter* counter, int chromaticNumber) {
    if (um) {
        if (counter->conditionVertices != 0) {
            return 1;
        } else {
            for (int i = 0; i < chromaticNumber; i++) {
                if (counter->condition[i] != counter->maxColoringMask) {
                    return 1;
                }
            }
        }
    } else if (coloring == PROPER || coloring == ODD) {
        return 0;
    } else if (open && proper) {
        return counter->conditionVertices != 0;
    }
    return 0;
}


void getColoringAfterCheck(counter* counter, int chromaticNumber, int colors[]) {
    if (!isConditionMet(counter, chromaticNumber)) {
        fprintf(stderr, "Requested coloring after check when condition isn't met.");
        exit(1);
    }
    if (um) {
        // We first check if a vertex always has a certain color as this is more important
        if (counter->conditionVertices != 0) {
            FOR_EACH_BIT(index, counter->conditionVertices) {
                for (int i = 0; i < chromaticNumber; i++) {
                    if ((counter->condition[i] & SHIFTL(index)) != 0) {
                        // It always has the color i + 1
                        colors[index] = i + 1;
                        break;
                    }
                }
            }
            snprintf(counter->extraInfo, MAX_STRING_LENGTH, " Vertex always has this color");
        } else {
            for (int i = 0; i < chromaticNumber; i++) {
                if (counter->condition[i] != counter->maxColoringMask) {
                    FOR_EACH_BIT(index, (counter->maxColoringMask & ~counter->condition[i])) {
                        colors[index] = i + 1;
                    }
                    break;
                }
            }
            snprintf(counter->extraInfo, MAX_STRING_LENGTH, " Vertex never has this color");
        }
    } else if (coloring == PROPER || coloring == ODD) {
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " ");
    } else if (proper && open) {
        FOR_EACH_BIT(index, counter->conditionVertices) {
            colors[index] = 1;
        }
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " Vertex always sees %d colors", chromaticNumber - 1);
    } else {
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " ");
    }
}


// The coloring should have already been asked for
char* getExtraInfoText(counter* counter) {
    return counter->extraInfo;
}