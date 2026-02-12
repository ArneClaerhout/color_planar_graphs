#include "counter.h"

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "vertex.h"

int inputColors(counter* counter, int colors[], int allColors, int chromaticNumber) {
    uint64_t oldCondition = 0;
    for (int k = 0; k < counter->numberOfVertices; k++) {
        // Here we add
        if (counter->checkAlwaysColor) {
            oldCondition = counter->condition[colors[k] - 1];
            counter->condition[colors[k] - 1] |= SHIFT(k);
            if (oldCondition != 0 && oldCondition != counter->condition[colors[k] - 1]) {
                // The vertex already had a color, and it wasn't this one
                counter->conditionVertices &= ~SHIFT(k);
            }
        } else {
            // When checking if a vertex is never a color, the above process would be too time-consuming
            counter->condition[colors[k] - 1] |= SHIFT(k);
        }

    }
    if (allColors) {
        fprintf(stderr, "Displaying all colorings is currently not implemented.");
        exit(1);
    } else {
        // We check if all of them are full, if they are: stop
        for (int i = 0; i < chromaticNumber; i++) {
            if (counter->condition[i] != counter->maxColoringMask) {
                return 1;
            }
        }
        return 0;
    }

}

int isConditionMet(counter* counter, int chromaticNumber) {
    for (int i = 0; i < chromaticNumber; i++) {
        if (counter->condition[i] != counter->maxColoringMask) {
            return 1;
        }
    }
    return 0;
}


void getColoringAfterCheck(counter* counter, int chromaticNumber, int colors[]) {
    if (!isConditionMet(counter, chromaticNumber)) {
        fprintf(stderr, "Requested coloring after check when condition isn't met.");
        exit(1);
    }
    if (counter->checkAlwaysColor) {
        FOR_EACH_BIT(index, counter->conditionVertices) {
            for (int i = 0; i < chromaticNumber; i++) {
                if ((counter->condition[i] & SHIFTL(index)) != 0) {
                    // It always has the color i + 1
                    colors[index] = i + 1;
                    break;
                }
            }
        }
    } else {
        for (int i = 0; i < chromaticNumber; i++) {
            if (counter->condition[i] != counter->maxColoringMask) {
                FOR_EACH_BIT(index, (counter->maxColoringMask & ~counter->condition[i])) {
                    colors[index] = i + 1;
                }
                break;
            }
        }
    }
}
