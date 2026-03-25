#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vertex.h"
#include "graph.h"
#include "types.h"
#include "main.h"

int index1iCFc = 0;
int index2iCFc = 0;

/**
 * Starts up a counter for the current graph.
 * If a counter already exists, it gets reused.
 *
 * @param n The number of vertices this counter will keep track of.
 */
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

/**
 * Inputs the given coloring into the given counter.
 *
 * @param counter The counter to input the coloring into.
 * @param colors The coloring of a graph to input.
 * @param allColors Whether all colorings should get input. Currently not supported.
 * @param chromaticNumber The chromatic number of the graph.
 * @return One, if the inputting of colorings can stop (because the condition isn't met).
 *         Zero, otherwise.
 */
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
        // This is CF coloring (only useful for pCFo)
        for (int k = 0; k < counter->numberOfVertices; k++) {
            int colorsForVertex = 0;
            FOR_EACH_BIT(index, g->verticesIndexed[k].neighbors) {
                colorsForVertex ^= SHIFT(g->verticesIndexed[index].color);
            }
            if (__builtin_popcount(colorsForVertex) != chromaticNumber - 1) {
                counter->conditionVertices &= ~SHIFTL(k);
            }
        }
        if (counter->conditionVertices == 0) {
            return 1;
        }
        return 0;
    }
    else if (isProperColoring && isOpenColoring) {
        // This is CF coloring (only useful for pCFo)
        for (int k = 0; k < counter->numberOfVertices; k++) {
            int colorsForVertex = 0;
            FOR_EACH_BIT(index, g->verticesIndexed[k].neighbors) {
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
                }
            }
        }
        return 0;
    } else {
        return 1;
    }

}

/**
 * Checks whether the condition has been met after inputting all the colorings into counter.
 *
 * @param counter The counter used to check for the condition.
 * @param chromaticNumber The chromatic number of the graph.
 * @return One, if the condition is met.
 *         Zero, otherwise.
 */
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
        return counter->conditionVertices != 0;
    } else if (isOpenColoring && isProperColoring) {
        return counter->conditionVertices != 0;
    } else if (!isOpenColoring && !isProperColoring) {
        for (int j = 1; j < counter->numberOfVertices; j++) {
            if (counter->condition[j] != counter->maxColoringMask) {
                // We check all the possible vertices
                for (int index2 = 0; index2 < j; index2++) {
                    // This index has different color than j
                    if (counter->condition[j] & SHIFTL(index2)) {

                        // We find two that aren't equal
                        index1iCFc = j;
                        index2iCFc = index2;

                        // We get the chromatic number of the graph now to compare later
                        int c = g->chromaticNumber;

                        // We reset the graph, and colour the two vertices
                        g->availableVertices = g->maxColoringMask;
                        g->availableVertices &= ~SHIFTL(index1iCFc);
                        g->availableVertices &= ~SHIFTL(index2iCFc);
                        // We give them the same color
                        g->verticesIndexed[index1iCFc].color = 1;
                        g->verticesIndexed[index2iCFc].color = 1;
                        // And now check, using a special colorChecker
                        colorCheck = &colorCheckiCFc;
                        for (int j = 0; j < g->numberOfVertices; j++) {
                            setMaxAvailableColors(&g->verticesIndexed[j], 3);
                        }
                        if (optimizedAlgorithm(0, 3, 0, 0, 0)) {

                            // We don't forget to change the colorCheck function back
                            colorCheck = &isCorrectlyColoredCF;

                            // The graph was colored with only 3 colors, while the two vertices are of equal color
                            int colors[g->numberOfVertices];
                            getColors(colors);
                            printColors(colors);
                        } else {

                            // fprintf(stderr, "index1: %d, index2: %d\n", index1iCFc, index2iCFc);

                            // We don't forget to change the colorCheck function back
                            colorCheck = &isCorrectlyColoredCF;

                            // If we weren't able to color it when both are the same color
                            // Even if we don't check these vertices in colorCheck
                            // The condition is met
                            return 1;
                        }


                    }
                }

            }
        }
    }
    return 0;
}

/**
 * A helper function used to alternatively color in a graph using this special color checker method.
 *
 * @param v The vertex that is going to get checked for correctness of coloring.
 * @param verticesIndexed The array of vertices in the current graph.
 * @return One, if the vertex is correctly colored.
 *         Zero, otherwise.
 */
int colorCheckiCFc(vertex* v, vertex verticesIndexed[]) {
    if (v == &g->verticesIndexed[index1iCFc] || v == &g->verticesIndexed[index2iCFc]) {
        return 1;
    }
    return isCorrectlyColoredCF(v, verticesIndexed);
}

/**
 * Finds the coloring after all possible colorings of the graph have been input into the counter.
 * This will set the given colors array to the coloring.
 * For most colorings, this will not return an actual coloring of the graph,
 * only a representation of what vertices were important in the checking of a condition.
 *
 * @param counter The counter that was used to input all colorings.
 * @param chromaticNumber The chromatic number of the graph.
 * @param colors The colors array used to write the coloring into.
 */
void getColoringAfterCheck(counter* counter, int chromaticNumber, int colors[]) {
    // if (!isConditionMet(counter, chromaticNumber)) {
    //     fprintf(stderr, "Requested coloring after check when condition isn't met.");
    //     exit(1);
    // }
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
        FOR_EACH_BIT(index, counter->conditionVertices) {
            colors[index] = 1;
        }
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " Vertex always sees %d colors a odd amount of times", chromaticNumber - 1);
    } else if (isProperColoring && isOpenColoring) {
        FOR_EACH_BIT(index, counter->conditionVertices) {
            colors[index] = 1;
        }
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " Vertex always sees %d colors", chromaticNumber - 1);
    } else if (!isProperColoring && !isOpenColoring) {
        // Before calling this method, we called the isConditionMet function
        // If it was, the last index1iCFc and index2iCFc will be correct
        colors[index1iCFc] = 1;
        colors[index2iCFc] = 1;
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " Vertices always have different colors");
    } else {
        snprintf(counter->extraInfo, MAX_STRING_LENGTH, " ");
    }
}


/**
 * Helper method for finding the extra info of the counter.
 * The coloring should have already been asked for.
 *
 * @param counter The counter to get the extra info of.
 * @return The extra info.
 */
char* getExtraInfoText(counter* counter) {
    return counter->extraInfo;
}