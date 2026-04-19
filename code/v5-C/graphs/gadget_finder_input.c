#include "gadget_finder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vertex.h"
#include "graph.h"
#include "gadget_finder_input.h"
#include "main.h"


int colorCheckiCFc(vertex* v, vertex verticesIndexed[]) {
    if (v == &verticesIndexed[index1iCFc] || v == &verticesIndexed[index2iCFc]) {
        return 1;
    }
    return isCorrectlyColoredCF(v, verticesIndexed);
}


char* getExtraInfoText(gadget_finder* gadget_finder) {
    return gadget_finder->extraInfo;
}

void startGadgetFinder(graph* g, int n) {
    if (checkCondition != 0) {
        gadget_finder* c;
        if (g->gadget_finder == NULL) {
            c = (gadget_finder*) malloc(sizeof(gadget_finder));
            g->gadget_finder = c;
        } else {
            c = g->gadget_finder;
        }
        c->firstInput = 1;
        c->numberOfVertices = n;
        c->maxColoringMask = SHIFTL(n) - 1;
        c->conditionVertices = c->maxColoringMask;
        memset(c->condition, 0, sizeof(c->condition));
    }
}


int inputColors(graph* g, const int colors[], int chromaticNumber) {
    gadget_finder* gadget_finder = g->gadget_finder;
    if (isUMColoring) {
        bitset_t oldCondition = 0;
        for (int k = 0; k < gadget_finder->numberOfVertices; k++) {
            // Here we add
            oldCondition = gadget_finder->condition[colors[k] - 1];
            gadget_finder->condition[colors[k] - 1] |= SHIFTL(k);
            if (!gadget_finder->firstInput && oldCondition != gadget_finder->condition[colors[k] - 1]) {
                // The vertex already had a color, and it wasn't this one
                gadget_finder->conditionVertices &= ~SHIFTL(k);
            }
        }
        if (gadget_finder->firstInput) gadget_finder->firstInput = 0;
        if (checkCondition == 1 && gadget_finder->conditionVertices == 0) return 1;
        // We check if all of them are full, if they are: stop
        for (int i = 0; i < chromaticNumber; i++) {
            if (gadget_finder->condition[i] != gadget_finder->maxColoringMask) {
                return 0;
            }
        }
        return 1;
    } else if (coloring == PROPER) {
        // We can do the iCFo coloring method
        if (g->chromaticNumber == 4) {
            subdivide(g, checkCondition != 1);
            // We fully reset the graph
            resetGraph(g, g->numberOfVertices);

            // We briefly change the coloring
            coloring = iCFo; isProperColoring = 0; isOpenColoring = 1; isUMColoring = 0;

            int c = findChromaticNumberOptimized(g, 4);
            if (c > 4) {
                gadget_finder->conditionVertices = 1;
            } else {
                gadget_finder->conditionVertices = 0;
            }

            // We change the coloring back to the original
            coloring = PROPER; isProperColoring = 1; isOpenColoring = 1; isUMColoring = 0;
            return 1;
        } else {
            gadget_finder->conditionVertices = 0;
            return 1;
        }
    } else if (coloring == ODD) {
        // This is CF coloring (only useful for pCFo)
        for (int k = 0; k < gadget_finder->numberOfVertices; k++) {
            int colorsForVertex = 0;
            FOR_EACH_BIT(index, g->verticesIndexed[k].neighbors) {
                colorsForVertex ^= SHIFT(g->verticesIndexed[index].color);
            }
            if (__builtin_popcount(colorsForVertex) != chromaticNumber - 1) {
                gadget_finder->conditionVertices &= ~SHIFTL(k);
            }
        }
        if (gadget_finder->conditionVertices == 0) {
            return 1;
        }
        return 0;
    }
    else if (isProperColoring && isOpenColoring) {
        // This is CF coloring (only useful for pCFo)
        for (int k = 0; k < gadget_finder->numberOfVertices; k++) {
            int colorsForVertex = 0;
            FOR_EACH_BIT(index, g->verticesIndexed[k].neighbors) {
                colorsForVertex |= SHIFT(g->verticesIndexed[index].color);
            }
            if (__builtin_popcount(colorsForVertex) != chromaticNumber - 1) {
                gadget_finder->conditionVertices &= ~SHIFTL(k);
            }
        }
        if (gadget_finder->conditionVertices == 0) {
            return 1;
        }
        return 0;
    } else if (!isProperColoring && !isOpenColoring) {
        // Here we check if we find two vertices that never have the same color
        for (int k = 0; k < gadget_finder->numberOfVertices; k++) {
            for (int j = 0; j < gadget_finder->numberOfVertices; j++) {
                if (colors[k] == colors[j]) {
                    gadget_finder->condition[k] |= SHIFTL(j);
                    gadget_finder->condition[j] |= SHIFTL(k);
                }
            }
        }
        return 0;
    } else {
        return 1;
    }

}


int isConditionMet(graph* g, int chromaticNumber) {
    gadget_finder* gadget_finder = g->gadget_finder;
    if (isUMColoring) {
        if ((checkCondition == 1 || checkCondition == 3) && gadget_finder->conditionVertices != 0) {
            return 1;
        } else if (checkCondition != 1) {
            for (int i = 0; i < chromaticNumber; i++) {
                if (gadget_finder->condition[i] != gadget_finder->maxColoringMask) {
                    return 1;
                }
            }
        }
    } else if (coloring == PROPER) {
        return gadget_finder->conditionVertices != 0;
    } else if (coloring == ODD) {
        return gadget_finder->conditionVertices != 0;
    } else if (isOpenColoring && isProperColoring) {
        return gadget_finder->conditionVertices != 0;
    } else if (!isOpenColoring && !isProperColoring) {
        for (int j = 1; j < gadget_finder->numberOfVertices; j++) {
            if (gadget_finder->condition[j] != gadget_finder->maxColoringMask) {
                // We check all the possible vertices
                for (int index2 = 0; index2 < j; index2++) {
                    // This index has different color than j
                    if (gadget_finder->condition[j] & SHIFTL(index2)) {

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
                        if (optimizedAlgorithm(g, 0, 3, 0, 0)) {

                            // We don't forget to change the colorCheck function back
                            colorCheck = &isCorrectlyColoredCF;

                            // The graph was colored with only 3 colors, while the two vertices are of equal color
                            int colors[g->numberOfVertices];
                            getColors(g, colors);
                            printColors(g, colors);
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


void getColoringAfterCheck(graph* g, int chromaticNumber, int colors[]) {
    gadget_finder* gadget_finder = g->gadget_finder;
    // if (!isConditionMet(gadget_finder, chromaticNumber)) {
    //     fprintf(stderr, "Requested coloring after check when condition isn't met.");
    //     exit(1);
    // }
    if (isUMColoring) {
        // We first check if a vertex always has a certain color as this is more important
        if ((checkCondition == 1 || checkCondition == 3) && gadget_finder->conditionVertices != 0) {
            FOR_EACH_BIT(index, gadget_finder->conditionVertices) {
                for (int i = 0; i < chromaticNumber; i++) {
                    if ((gadget_finder->condition[i] & SHIFTL(index)) != 0) {
                        // It always has the color i + 1
                        colors[index] = i + 1;
                        break;
                    }
                }
            }
            snprintf(gadget_finder->extraInfo, MAX_STRING_LENGTH, " Vertex always has this color");
        } else {
            for (int i = 0; i < chromaticNumber; i++) {
                if (gadget_finder->condition[i] != gadget_finder->maxColoringMask) {
                    FOR_EACH_BIT(index, (gadget_finder->maxColoringMask & ~gadget_finder->condition[i])) {
                        colors[index] = i + 1;
                    }
                    break;
                }
            }
            snprintf(gadget_finder->extraInfo, MAX_STRING_LENGTH, " Vertex never has this color");
        }
    } else if (coloring == PROPER) {
        getColors(g, colors);
        snprintf(gadget_finder->extraInfo, MAX_STRING_LENGTH, " ");
    } else if (coloring == ODD) {
        FOR_EACH_BIT(index, gadget_finder->conditionVertices) {
            colors[index] = 1;
        }
        snprintf(gadget_finder->extraInfo, MAX_STRING_LENGTH, " Vertex always sees %d colors a odd amount of times", chromaticNumber - 1);
    } else if (isProperColoring && isOpenColoring) {
        FOR_EACH_BIT(index, gadget_finder->conditionVertices) {
            colors[index] = 1;
        }
        snprintf(gadget_finder->extraInfo, MAX_STRING_LENGTH, " Vertex always sees %d colors", chromaticNumber - 1);
    } else if (!isProperColoring && !isOpenColoring) {
        // Before calling this method, we called the isConditionMet function
        // If it was, the last index1iCFc and index2iCFc will be correct
        colors[index1iCFc] = 1;
        colors[index2iCFc] = 1;
        snprintf(gadget_finder->extraInfo, MAX_STRING_LENGTH, " Vertices always have different colors");
    } else {
        snprintf(gadget_finder->extraInfo, MAX_STRING_LENGTH, " ");
    }
}