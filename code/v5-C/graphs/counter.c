#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vertex.h"
#include "graph.h"
#include "types.h"

extern int isUMColoring;
extern int isProperColoring;
extern int isOpenColoring;
extern enum colorings coloring;
extern graph* g;
extern int checkCondition;

void startCounter(int n) {
    if (checkCondition != 0) {
        counter* c;
        if (g->counter == NULL) {
            c = (counter*) malloc(sizeof(counter));
            g->counter = c;
        } else {
            c = g->counter;
        }
        c->firstInput = 1;
        c->numberOfVertices = n;
        c->maxColoringMask = SHIFTL(n) - 1;
        c->conditionVertices = c->maxColoringMask;
        memset(c->condition, 0, sizeof(c->condition));
    }
}

int inputColors(counter* counter, const int colors[], int allColors, int chromaticNumber) {
    if (allColors) {
        fprintf(stderr, "Displaying all colorings is currently not implemented.");
        exit(1);
    }
    if (isUMColoring) {
        bitset_t oldCondition = 0;
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
        if (checkCondition == 1 && counter->conditionVertices == 0) return 1;
        // We check if all of them are full, if they are: stop
        for (int i = 0; i < chromaticNumber; i++) {
            if (counter->condition[i] != counter->maxColoringMask) {
                return 0;
            }
        }
        return 1;
    } else if (coloring == PROPER) {
        // We can do the iCFo coloring method
        if (g->chromaticNumber == 4) {
            subdivide(checkCondition != 1);
            // We fully reset the graph
            resetGraph(g->numberOfVertices);

            // We briefly change the coloring
            coloring = iCFo; isProperColoring = 0; isOpenColoring = 1; isUMColoring = 0;

            int c = findChromaticNumberOptimized(4, 0);
            if (c > 4) {
                counter->conditionVertices = 1;
            } else {
                counter->conditionVertices = 0;
            }

            // We change the coloring back to the original
            coloring = PROPER; isProperColoring = 1; isOpenColoring = 1; isUMColoring = 0;
            return 1;
        } else {
            counter->conditionVertices = 0;
            return 1;
        }
    } else if (coloring == ODD) {
        return 1;
    }
    else if (isProperColoring && isOpenColoring) {
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
    } else if (!isProperColoring && !isOpenColoring) {
        // Here we check if we find two vertices that never have the same color
        for (int k = 0; k < counter->numberOfVertices; k++) {
            for (int j = 0; j < counter->numberOfVertices; j++) {
                if (colors[k] == colors[j]) {
                    counter->condition[k] |= SHIFTL(j);
                    counter->condition[j] |= SHIFTL(k);
                } else {
                    int neighbourColor = g->verticesIndexed[j].color;
                    int correct = 0;
                    FOR_EACH_BIT(index, g->verticesIndexed[k].neighbours) {
                        if (index != j && g->verticesIndexed[index].color == neighbourColor) {
                            correct = 1;
                            break;
                        }
                    }
                    if (!correct) {
                        counter->condition[k] |= SHIFTL(j);
                        counter->condition[j] |= SHIFTL(k);
                        continue;
                    }
                    neighbourColor = g->verticesIndexed[k].color;
                    correct = 0;
                    FOR_EACH_BIT(index, g->verticesIndexed[j].neighbours) {
                        if (index != k && g->verticesIndexed[index].color == neighbourColor) {
                            correct = 1;
                            break;
                        }
                    }
                    if (!correct) {
                        counter->condition[k] |= SHIFTL(j);
                        counter->condition[j] |= SHIFTL(k);
                        continue;
                    }

                }
            }
        }
        if (!isConditionMet(counter, chromaticNumber)) {
            return 1;
        }
        return 0;
    } else {
        return 1;
    }

}

int isConditionMet(counter* counter, int chromaticNumber) {
    if (isUMColoring) {
        if ((checkCondition == 1 || checkCondition == 3) && counter->conditionVertices != 0) {
            return 1;
        } else if (checkCondition != 1) {
            for (int i = 0; i < chromaticNumber; i++) {
                if (counter->condition[i] != counter->maxColoringMask) {
                    return 1;
                }
            }
        }
    } else if (coloring == PROPER) {
        return counter->conditionVertices != 0;
    } else if (coloring == ODD) {
        return 0;
    }
    else if (isOpenColoring && isProperColoring) {
        return counter->conditionVertices != 0;
    } else if (!isOpenColoring && !isProperColoring) {
        for (int j = 0; j < counter->numberOfVertices; j++) {
            if (counter->condition[j] != counter->maxColoringMask) {
                return 1;
            }
        }
    }
    return 0;
}


void getColoringAfterCheck(counter* counter, int chromaticNumber, int colors[]) {
    if (!isConditionMet(counter, chromaticNumber)) {
        fprintf(stderr, "Requested coloring after check when condition isn't met.");
        exit(1);
    }
    if (isUMColoring) {
        // We first check if a vertex always has a certain color as this is more important
        if ((checkCondition == 1 || checkCondition == 3) && counter->conditionVertices != 0) {
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
    } else if (coloring == PROPER) {
        getColors(colors);
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " ");
    } else if (coloring == ODD) {
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " ");
    } else if (isProperColoring && isOpenColoring) {
        FOR_EACH_BIT(index, counter->conditionVertices) {
            colors[index] = 1;
        }
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " Vertex always sees %d colors", chromaticNumber - 1);
    } else if (!isProperColoring && !isOpenColoring) {

        for (int j = 0; j < counter->numberOfVertices; j++) {
            if (counter->condition[j] != counter->maxColoringMask) {
                // fprintf(stderr, "%ld\n", counter->condition[j]);
                colors[j] = 1;
                colors[bitset_ctz(~counter->condition[j])] = 1;
                break;
            }
        }
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " Vertices always have different colors");
    } else {
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " ");
    }
}


// The coloring should have already been asked for
char* getExtraInfoText(counter* counter) {
    return counter->extraInfo;
}