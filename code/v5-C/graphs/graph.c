#include "colorings.h"
#include "graph.h"
#include "vertex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


extern int minChrom;
extern int checkCondition;
extern int open;
extern int proper;
extern int um;

extern graph* g;
extern int lengthOfGraph;

extern int (*handler)(int, vertex*, int, uint64_t);
extern int (*colorCheck)(vertex*, vertex*, int);
extern enum colorings coloring;




void getColors(int colors[]) {
    for (int i = 0; i < g->numberOfVertices; i++) {
        colors[i] = g->verticesIndexed[i].color;
    }
}

int getNumberOfVertices(char graphString[]) {
    int index = 0;
    // Only works when #vertices < 63, we won't go higher than that
    if (graphString[index] < 126) {
        return (int) graphString[index] - 63;
    }
    return 0;
}

graph* createGraph(int previousN, char graphString[]) {
    int n = getNumberOfVertices(graphString);

    if (previousN < n) {
        if (previousN == 0) {
            lengthOfGraph = sizeof(graph) + n * sizeof(vertex);
            g = (graph*) malloc(lengthOfGraph);
            g->changed = malloc(sizeof(uint64_t[n][10]));
        } else {
            lengthOfGraph += (n - previousN) * (int) sizeof(vertex);
            graph* temp = realloc(g, lengthOfGraph);
            if (temp != NULL) {
                g = temp;
            } else {
                fprintf(stderr, "Realloc can't be executed: memory full.");
                exit(1);
            }

            uint64_t (*temp2)[10] = realloc(g->changed, sizeof(uint64_t[n][10]));
            if (temp2 != NULL) {
                g->changed = temp2;
            } else {
                fprintf(stderr, "Realloc can't be executed: memory full.");
                exit(1);
            }
        }

        g->numberOfVertices = n;
        g->maxColoringMask = SHIFTL(g->numberOfVertices) - 1;
    }

    // Set the changed 2D-array to zeroes
    memset(g->changed,0, sizeof(uint64_t[n][10]));

    g->chromaticNumber = 0;

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
        memset(c->condition, 0, sizeof(uint64_t[10]));
        if (checkCondition == 1) {
            c->checkAlwaysColor = 1;
        } else if (checkCondition == 2) {
            c->checkAlwaysColor = 0;
        }
    }

    for (int i = 0; i < n; i++) {
        g->verticesIndexed[i].index = i;
        g->verticesIndexed[i].neighbours = 0;
        g->verticesIndexed[i].color = 0;

    }
    int index = 1; // First index as index 0 is the vertex count

    int bitPos = 0; // Bit position per index
    for (int i = 1; i < n; i++) {
        for (int j = 0; j < i; j++) {
            // Go through the entire adjacency matrix
            int part6bits = graphString[index] - 63;

            int bitShift = 5 - bitPos;
            // The amount you have to bitshift the number to check bitPos here:
            int edge = ((part6bits >> bitShift) & 1) == 1;
            if (edge) {
                addNeighbour(&g->verticesIndexed[i], &g->verticesIndexed[j]);
                // The matrix is symmetrical
            }

            bitPos++;
            if (bitPos == 6) {
                index++;
                bitPos = 0;
            }
        }
    }

    return g;

}

int findChromaticNumberOptimized(int startingColor, int allColorings) {
    for (int i = startingColor; i <= 10; i++) {
        for (int j = 0; j < g->numberOfVertices; j++) {
            setMaxAvailableColors(&g->verticesIndexed[j], i);
        }
        // Each time we reset which vertices are colored.
        g->vertexIsColored = 0;
        g->availableVertices = g->maxColoringMask;
        if (optimizedAlgorithm(0, i, 0, allColorings, 0)) {
            g->chromaticNumber = i;
            return i;
        }
        if ((allColorings || checkCondition != 0) && g->chromaticNumber != 0) {
            g->chromaticNumber = i;
            return i; // we wanted to find all colorings
        }
    }
    return 0;
}

int optimizedAlgorithm(int maxColorCurrGraph, int maxColor, int index, int allColorings, int depth) {

    if (g->vertexIsColored == g->maxColoringMask) {
        return startingStep(maxColor, allColorings);
    }

    vertex* v = &g->verticesIndexed[index];
    int colors = v->availableColors;

    int maxLoop = (um || checkCondition != 0 || allColorings) ? maxColor : min(maxColorCurrGraph + 1, maxColor);
    // Every coloring should be tried for um, as this is different for it.

    // We are coloring this index
    g->vertexIsColored |= 1L << index;
    // vertexIsAlmostColored &= ~(1 << index);

    int lastToColor = (g->maxColoringMask & ~g->vertexIsColored) == 0;
    if (lastToColor && maxLoop < maxColor)
        colors = 0; // We don't want to go over colors, as it can't be correct

    uint64_t neighbourhood = v->neighbours;
    int neighboursColored = (neighbourhood & g->vertexIsColored) == neighbourhood;

    FOR_EACH_BIT(color, colors) {
        if (color > maxLoop)
            break; // We passed the highest possible color in the graph

        v->color = color + 1; // + 1 as the actual colors are from 1 to n

        // We have to now check if all our neighbours are colored, as this isn't checked
        // in updateNeighbours
        // This is an extra check for correctness
        if (neighboursColored && !colorCheck(v, g->verticesIndexed, 0)) {
            // This color isn't correct, we pick another
            continue;
        }
        // We also change the available colors for the neighbours if the coloring is
        // proper
        if (updateNeighbours(v, color, depth)) {
            // This is used to skip this color, as it isn't possible
            continue;
        }

        int newMaxColorCurrGraph = max(maxColorCurrGraph, color + 1);
        int newIndex = getBestIndex(index);
        if (optimizedAlgorithm(newMaxColorCurrGraph, maxColor, newIndex, allColorings, depth + 1)) {
            return 1;
        }

        // We add back the available colors if it didn't work out
        addColorsBack(depth);
    }

    // We decolor this vertex
    g->vertexIsColored &= ~(1L << index);
    v->color = 0;
    g->availableVertices |= (1L << index);

    return 0;
}



int startingStep(int maxColor, int allColorings) {
    if (checkCondition != 0 || allColorings) {
        if (maxColor < minChrom) {
            // When it's going to get filtered away anyway, we stop it
            return 1;
        }
        g->chromaticNumber = maxColor;
        // If the counters are all full, return true and stop counting
        // Otherwise, this will return false and we keep trying
        int colors[g->numberOfVertices];
        getColors(colors);
        return inputColors(g->counter, colors, allColorings, g->chromaticNumber);
    }
    return 1;
}


int getBestIndex(int indexColored) {
    // The addition of tiebreaks with degree only slows it down
    // taking the first best vertex is fastest
    if (indexColored != -1)
        g->availableVertices &= ~(SHIFTL(indexColored));
    int bestIndex = 0;
    int smallestAC = 99;
    FOR_EACH_BIT(index, g->availableVertices) {
        int testAC = g->verticesIndexed[index].amountOfAvailableColors;
        if (testAC == 1)
            return index; // We break early as this is the best possible
        if (smallestAC > testAC) {
            smallestAC = testAC;
            bestIndex = index;
        }
    }
    return bestIndex;
}



int updateNeighbours(vertex* v, int color, int depth) {
    FOR_EACH_BIT(bit, v->neighbours) {
        vertex* neighbour = &g->verticesIndexed[bit];

        // neighbour.incrementAmountOfColoredNeighbours();

        int neighbourIsColored = (SHIFTL(bit) & g->vertexIsColored) != 0;

        uint64_t neighbourhood = neighbour->neighbours;
        neighbourhood = (open ? neighbourhood : (neighbourhood | SHIFTL(bit)));

        uint64_t diff = neighbourhood & ~g->vertexIsColored;

        // We will allow adding vertices with one possible color that aren't colored
        // diff &= ~vertexIsAlmostColored;

        if (diff == 0 && (open || neighbourIsColored)) {
            // All the neighbour's neighbours are colored and the neighbour itself is
            // colored
            // We want to check if the neighbour is CORRECTLY colored
            if (!colorCheck(neighbour, g->verticesIndexed, 1)) {
                // Early pruning
                addColorsBack(depth);
                return 1;
                // We skip the rest, as this color is incorrect
            }
            // It doesn't have to be checked on whether it's proper, as this is done by the
            // next section
        } else if (diff != 0 && (diff & (diff - 1)) == 0) { // bitCount(diff) == 1
            int toColorNeighbourIndex = __builtin_ctz(diff);
            vertex* toColorNeighbour = &g->verticesIndexed[toColorNeighbourIndex];
            // There's one vertex that isn't colored yet.
            if (neighbourIsColored || !proper) {
                if (handler(depth, toColorNeighbour, toColorNeighbourIndex, neighbourhood)) {
                    return 1;
                }
            }
            // Proper colorings are done later
        }

        if (proper) {
            // This section removes the color from the available colors

            // We check if the neighbour is already colored, as we don't have to do anything
            // if this is the case
            if (neighbourIsColored)
                continue;

            // We compare the neighbours neighbourhood with the already colored vertices
            if (removeColorMask(neighbour, neighbour->index, SHIFT(color), depth)) {
                return 1;
            }
        }
    }

    return 0;
}



void addColorsBack(int depth) {
    for (int i = 0; i < 10; i++) {
        uint64_t value = g->changed[depth][i];
        if (value != 0) {
            FOR_EACH_BIT(index, value) {
                vertex* changedNeighbour = &g->verticesIndexed[index];
                addColorFromAvailableColors(changedNeighbour, i);
            }
            g->changed[depth][i] = 0; // We reset changed
            // if (changedNeighbour.getAmountOfAvailableColors() == 1) {
            // vertexIsAlmostColored |= 1 << changedNeighbour.getIndex();
            // } else {
            // vertexIsAlmostColored &= ~(1 << changedNeighbour.getIndex());
            // }
        }
    }
}

int handleProper(int, vertex*, int, uint64_t) {
    return 0;
}

int handleCF(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood) {
    int colorsOccurOnce = 0;
    int colorsOccur = 0;

    int colorIndex;

    neighbourhood = neighbourhood & ~SHIFTL(toColorNeighbourIndex);
    FOR_EACH_BIT(index, neighbourhood) {
        vertex *secondNeighbour = &g->verticesIndexed[index];
        int neighbourColor = secondNeighbour->color;
        // We do -1 as the colors are from 1...k,
        // but we want to later on use the colors 0...k-1

        if (neighbourColor == 0) { // We now for a fact that this vertex has one color available, as this was
            // checked beforehand
            colorIndex = SHIFT(__builtin_ctz(secondNeighbour->availableColors));
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

    if (colorsOccurOnce == 0) {
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, colorsOccur, depth);
    } else if ((colorsOccurOnce & (colorsOccurOnce - 1)) == 0) { // bitCount == 1
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, colorsOccurOnce, depth);
    }
    return 0;
}


int handleUM(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood) {
    int max = 1;
    int amountOfMax = 0;

    neighbourhood = neighbourhood & ~SHIFTL(toColorNeighbourIndex);
    FOR_EACH_BIT(index, neighbourhood) {
        vertex *secondNeighbour = &g->verticesIndexed[index];
        int neighbourColor = secondNeighbour->color;

        if (neighbourColor == 0) { // We now for a fact that this vertex has one color available, as this was
            // checked beforehand
            neighbourColor = __builtin_ctz(secondNeighbour->availableColors) + 1;
        }

        if (neighbourColor == max) {
            amountOfMax++;
        } else if (neighbourColor > max) {
            max = neighbourColor;
            amountOfMax = 1;
        }
    }


    // We need to find the maximum and the amount of times it occurs
    // If it occurs once, don't use this color, if it occurs more than once
    // Only allow colors bigger than the max
    // Otherwise, do nothing.
    if (amountOfMax == 1) {
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, SHIFT((max - 1)), depth);
    } else if (amountOfMax > 1) {
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, SHIFT(max) - 1, depth);
    }
    return 0;
}


int handleOdd(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood) {
    int colorsOccurOdd = 0;

    neighbourhood = neighbourhood & ~SHIFTL(toColorNeighbourIndex);
    FOR_EACH_BIT(index, neighbourhood) {
        vertex *secondNeighbour = &g->verticesIndexed[index];
        int neighbourColor = secondNeighbour->color;

        if (neighbourColor == 0) { // We now for a fact that this vertex has one color available, as this was
            // checked beforehand
            colorsOccurOdd ^= SHIFT(__builtin_ctz(secondNeighbour->availableColors));;
        } else {
            colorsOccurOdd ^= SHIFT((neighbourColor - 1));
            // We do -1 as the colors are from 1...k,
            // but we want to later on use the colors 0...k-1
        }
    }

    if (colorsOccurOdd != 0 && (colorsOccurOdd & (colorsOccurOdd - 1)) == 0) { // Check whether there's one bit
        // Don't take this color
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, colorsOccurOdd, depth);
    }
    return 0;
}


int removeColor(vertex* v, int index, int color, int depth) {
    if (!removeColorFromAvailableColors(v, color))
        return 0;
    g->changed[depth][color] |= SHIFTL(index);
    int m = v->amountOfAvailableColors;
    if (m == 0) {
        addColorsBack(depth);
        return 1;
    }
    // else if (m == 1) {
    // vertexIsAlmostColored |= (1 << index);
    // }
    return 0;
}

int removeColorMask(vertex* v, int index, int color, int depth) {
    if ((v->availableColors & color) == 0) {
        return 0;
    }
    if (v->availableColors == (v->availableColors & color)) {
        // It will remove all available colors
        addColorsBack(depth);
        return 1;
    }
    // We do a special remove colors from available colors
    FOR_EACH_BIT(colorIndex, (v->availableColors & color)) {
        g->changed[depth][colorIndex] |= SHIFTL(index);
    }
    v->availableColors &= ~color;
    v->amountOfAvailableColors = __builtin_popcount(v->availableColors);
    // else if (m == 1) {
    // vertexIsAlmostColored |= (1 << index);
    // }
    return 0;
}



