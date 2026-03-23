#include "colorings.h"
#include "graph.h"
#include "vertex.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


extern int minChrom;
extern int checkCondition;
extern int isOpenColoring;
extern int isProperColoring;
extern int isUMColoring;

extern graph* g;
extern int lengthOfGraph;

extern int (*handler)(int, vertex*, int, bitset_t, int);
extern int (*colorCheck)(vertex*, vertex*);
extern enum colorings coloring;




void getColors(int colors[]) {
    for (int i = 0; i < g->numberOfVertices; i++) {
        colors[i] = g->verticesIndexed[i].color;
    }
}

// Returns the number of vertices and updates dataStart
// so you know where the edge bits begin.
int getNumberOfVertices(char graphString[], int *dataStart) {
    if (graphString[0] < 126) {
        *dataStart = 1;
        return (int) graphString[0] - 63;
    }

    // Check if it's the 3-byte version (n <= 258047)
    if (graphString[0] == 126 && graphString[1] < 126) {
        *dataStart = 4;
        return ((graphString[1] - 63) << 12) |
               ((graphString[2] - 63) << 6)  |
               ((graphString[3] - 63));
    }
    return 0;
}

graph* createGraph(int previousN, char graphString[]) {
    int dataStart;
    int n = getNumberOfVertices(graphString, &dataStart);

    if (previousN == 0) {
        // There wasn't a graph before, we create one
        g = (graph*) malloc(sizeof(graph));
        g->changed = malloc(sizeof(bitset_t[MAX_VERTICES][10]));
        for (int i = 0; i < MAX_VERTICES; i++) {
            g->verticesIndexed[i].index = i;
        }
    }

    resetGraph(n);

    for (int i = 0; i < n; i++) {
        g->verticesIndexed[i].neighbours = 0;
    }

    int index = dataStart; // First index as index the first indices are the number of vertices

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

void resetGraph(int n) {
    g->numberOfVertices = n;
    g->maxColoringMask = SHIFTL(g->numberOfVertices) - 1;
    g->chromaticNumber = 0;

    // Set the changed 2D-array to zeroes
    memset(g->changed,0, sizeof(bitset_t[n][10]));

    startCounter(n);

    // for (int i = 0; i < n; i++) {
    //     g->verticesIndexed[i].color = 0;
    // }
}

int findChromaticNumberOptimized(int startingColor, int allColorings) {
    for (int i = startingColor; i <= 10; i++) {
        // fprintf(stderr, "%d\n", i);
        for (int j = 0; j < g->numberOfVertices; j++) {
            setMaxAvailableColors(&g->verticesIndexed[j], i);
        }
        // Each time we reset which vertices are colored.
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

    if (g->availableVertices == 0) {
        return startingStep(maxColor, allColorings);
    }

    vertex* v = &g->verticesIndexed[index];
    int colors = v->availableColors;

    // Every coloring should be tried for um, as this is different then other colorings
    int maxLoop = (isUMColoring || checkCondition != 0 || allColorings) ? maxColor : min(maxColorCurrGraph + 1, maxColor);

    // This vertex isn't available any more
    g->availableVertices &= ~(SHIFTL(index));

    FOR_EACH_BIT(color, colors) {
        if (color > maxLoop)
            break; // We passed the highest possible color in the graph

        v->color = color + 1; // + 1 as the actual colors are from 1 to n

        int newMaxColorCurrGraph = max(maxColorCurrGraph, color + 1);

        /**
        There is no check here for when all neighbours are colored.
        This is made sure by updateNeighbours of previous iterations
        **/

        // We also change the available colors for the neighbours
        if (updateNeighbours(v, color, depth, newMaxColorCurrGraph)) {
            // This is used to skip this color, as it isn't possible
            continue;
        }

        int newIndex = getBestIndex();
        if (optimizedAlgorithm(newMaxColorCurrGraph, maxColor, newIndex, allColorings, depth + 1)) {
            return 1;
        }

        // We add back the available colors if it didn't work out
        addColorsBack(depth, newMaxColorCurrGraph);
    }

    /**
    No Coloring was found.
    We decolor this vertex.
    **/
    // v->color = 0; // This actually doesn't matter
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


int getBestIndex() {
    // The addition of tiebreaks with degree only slows it down
    // taking the first best vertex is fastest
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



int updateNeighbours(vertex* v, int color, int depth, int maxColorInGraph) {
    FOR_EACH_BIT(bit, v->neighbours) {
        vertex* neighbour = &g->verticesIndexed[bit];

        int neighbourIsColored = (SHIFTL(bit) & g->availableVertices) == 0;

        // Neighbouring vertices can't have the same color in proper colorings:
        if (isProperColoring && !neighbourIsColored) {
            if (removeColorMask(neighbour, neighbour->index, SHIFT(color), depth, maxColorInGraph))
                return 1;
        }

        bitset_t neighbourhood = neighbour->neighbours;
        neighbourhood = (isOpenColoring ? neighbourhood : (neighbourhood | SHIFTL(bit)));
        bitset_t diff = neighbourhood & g->availableVertices;

        if (diff == 0) {
            // All the neighbour's neighbours are colored and the neighbour itself is colored
            // We want to check if the neighbour is CORRECTLY colored
            if (!colorCheck(neighbour, g->verticesIndexed)) {
                // Early pruning
                addColorsBack(depth, maxColorInGraph);
                return 1;
                // We skip the rest, as this color is incorrect
            }
        } else if ((diff & (diff - 1)) == 0) { // bitCount(diff) == 1
            // The one neighbour we still have to color:
            int toColorNeighbourIndex = bitset_ctz(diff);
            vertex* toColorNeighbour = &g->verticesIndexed[toColorNeighbourIndex];

            if (handler(depth, toColorNeighbour, toColorNeighbourIndex, neighbourhood, maxColorInGraph)) {
                return 1;
            }
        }
    }

    return 0;
}



void addColorsBack(int depth, int maxColorInGraph) {
    for (int i = 0; i < maxColorInGraph; i++) {
        bitset_t value = g->changed[depth][i];
        if (value != 0) {
            FOR_EACH_BIT(index, value) {
                vertex* changedNeighbour = &g->verticesIndexed[index];
                // addColorFromAvailableColors(changedNeighbour, i);
                // We know for sure that this was removed,
                // we don't have to do the check performed in the addColorFromAvailableColors method
                changedNeighbour->availableColors |= SHIFT(i);
                changedNeighbour->amountOfAvailableColors++;
            }
            g->changed[depth][i] = 0; // We reset changed
        }
    }
}

int handleProper(int, vertex*, int, bitset_t, int) {
    return 0;
}

int handleCF(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, bitset_t neighbourhood, int maxColorInGraph) {
    int colorsOccurOnce = 0;
    int colorsOccur = 0;

    neighbourhood = neighbourhood & ~SHIFTL(toColorNeighbourIndex);
    FOR_EACH_BIT(index, neighbourhood) {
        int colorIndex = SHIFT((g->verticesIndexed[index].color - 1));
        // We do -1 as the colors are from 1...k,
        // but we want to later on use the colors 0...k-1

        if ((colorsOccur & colorIndex) != 0) {
            // The color has already occurred, it can never occur once
            colorsOccurOnce &= ~colorIndex;
        } else {
            // The color hasn't occurred
            colorsOccurOnce |= colorIndex;
            colorsOccur |= colorIndex;
        }
    }

    if (colorsOccurOnce == 0) {
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, colorsOccur, depth, maxColorInGraph);
    } else if ((colorsOccurOnce & (colorsOccurOnce - 1)) == 0) { // bitCount == 1
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, colorsOccurOnce, depth, maxColorInGraph);
    }
    return 0;
}


int handleUM(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, bitset_t neighbourhood, int maxColorInGraph) {
    int max = 1;
    int amountOfMax = 0;

    neighbourhood = neighbourhood & ~SHIFTL(toColorNeighbourIndex);
    FOR_EACH_BIT(index, neighbourhood) {
        int neighbourColor = g->verticesIndexed[index].color;

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
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, SHIFT((max - 1)), depth, maxColorInGraph);
    }
    // The number of max is greater than 1 (because it can't be 0, there is always a max)
    return removeColorMask(toColorNeighbour, toColorNeighbourIndex, SHIFT(max) - 1, depth, maxColorInGraph);
}


int handleOdd(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, bitset_t neighbourhood, int maxColorInGraph) {
    int colorsOccurOdd = 0;

    neighbourhood = neighbourhood & ~SHIFTL(toColorNeighbourIndex);
    FOR_EACH_BIT(index, neighbourhood) {
        vertex *secondNeighbour = &g->verticesIndexed[index];
        int neighbourColor = secondNeighbour->color;

        colorsOccurOdd ^= SHIFT((neighbourColor - 1));
        // We do -1 as the colors are from 1...k,
        // but we want to later on use the colors 0...k-1
    }

    if (colorsOccurOdd != 0 && (colorsOccurOdd & (colorsOccurOdd - 1)) == 0) { // Check whether there's one bit
        // Don't take this color
        return removeColorMask(toColorNeighbour, toColorNeighbourIndex, colorsOccurOdd, depth, maxColorInGraph);
    }
    return 0;
}

// This method also updates the changed list in the graph, while this wouldn't happen in vertex.c
int removeColorMask(vertex* v, int index, int color, int depth, int maxColorInGraph) {
    int changedColors = v->availableColors & color;
    if (changedColors == 0) {
        return 0;
    }
    if (v->availableColors == changedColors) {
        // It will remove all available colors
        addColorsBack(depth, maxColorInGraph);
        return 1;
    }
    int count = 0;
    // We do a special remove colors from available colors
    FOR_EACH_BIT(colorIndex, changedColors) {
        g->changed[depth][colorIndex] |= SHIFTL(index);
        count++;
    }
    v->availableColors &= ~color;
    v->amountOfAvailableColors -= count;
    return 0;
}

// Subdivide the graph so that each edge gets turned into a vertex and two edges.
void subdivide(int removeOriginalEdge) {
    // We also reset the graph during this process
    int count = g->numberOfVertices;
    int index = count; // Keeps track of where to add the newest subdivision
    for (int i = 0; i < g->numberOfVertices; i++) {
        vertex* v = &g->verticesIndexed[i];
        v->color=0;
        FOR_EACH_BIT(j, v->neighbours & (SHIFTL(i) - 1)) {
            vertex* neighbour = &g->verticesIndexed[j];
            // Make the new neighbour
            vertex* newNeighbour = &g->verticesIndexed[count];
            newNeighbour->index = count;
            newNeighbour->neighbours = 0;
            newNeighbour->color = 0;

            if (removeOriginalEdge) {
                removeNeighbour(v, neighbour);
            }
            addNeighbour(v, newNeighbour);
            addNeighbour(neighbour, newNeighbour);
            count++;
        }
    }
    g->numberOfVertices = count; // We update the number of vertices
    g->maxColoringMask = SHIFTL(g->numberOfVertices) - 1;
    g->availableVertices = g->maxColoringMask;
    startCounter(count);
}


// Add a given graph in graph6 format to the current graph g.
// The graph gets linked by taking one vertex in the given graph
// and putting that as a vertex in the current graph.
void addGraphToIndex(char graphString[], int indexInThisGraph, int indexInOwnGraph) {

    // Grab the number of vertices of the to add graph
    int dataStart;
    int nbOfNewVertices = getNumberOfVertices(graphString, &dataStart);

    // The intermediate vertex
    vertex* target = &g->verticesIndexed[indexInThisGraph];
    // The starting index in the current graph for adding new vertices
    int startingIndex = g->numberOfVertices;

    // Set the neighbours to 0
    for (int i = startingIndex; i < startingIndex + nbOfNewVertices - 1; i++) {
        g->verticesIndexed[i].neighbours = 0;
    }

    // First index as index the first indices are the number of vertices
    int index = dataStart;

    int bitPos = 0; // Bit position per index
    for (int i = 1; i < nbOfNewVertices; i++) {
        for (int j = 0; j < i; j++) {
            // Go through the entire adjacency matrix
            int part6bits = graphString[index] - 63;

            int bitShift = 5 - bitPos;
            // The amount you have to bitshift the number to check bitPos here:
            int edge = ((part6bits >> bitShift) & 1) == 1;
            if (edge) {
                /**
                We also need the vertices to come right after the original graph
                We keep track of an extra int

                If the index I or J have passed the indices we're linking, they should shift
                **/
                int addI = 0;
                if (i >= indexInOwnGraph) addI = -1;

                int addJ = 0;
                if (j >= indexInOwnGraph) addJ = -1;

                // If i or j is the linked vertex, link the target instead
                vertex* vertex1 = &g->verticesIndexed[startingIndex + i + addI];
                vertex* vertex2 = &g->verticesIndexed[startingIndex + j + addJ];

                // I is one of the vertices that are supposed to be linked
                if (i == indexInOwnGraph) {
                    vertex1 = target;
                }

                // J is one of the vertices that are supposed to be linked
                if (j == indexInOwnGraph) {
                    vertex2 = target;
                }

                // We add the two vertices as neighbours
                addNeighbour(vertex1, vertex2);
            }

            bitPos++;
            if (bitPos == 6) {
                index++;
                bitPos = 0;
            }
        }
    }

    // We reset the graph to recolor
    resetGraph(g->numberOfVertices + nbOfNewVertices - 1);
}


// A method that replaces an edge in the current graph g with a given graph in graph6 format
void replaceEdgeByGraph(char graphString[], int idxOneThisGraph, int idxTwoThisGraph, int idxOneOwnGraph, int idxTwoOwnGraph) {
    // The intermediate vertices
    vertex* targetA = &g->verticesIndexed[idxOneThisGraph];
    vertex* targetB = &g->verticesIndexed[idxTwoThisGraph];

    // Firstly, we remove the edge between the two vertices in this graph
    removeNeighbour(targetA, targetB);

    /**
    Now it's time to add the graph to the current graph:
    **/

    // Grab the number of vertices of the graph we're adding
    int dataStart;
    int nbOfNewVertices = getNumberOfVertices(graphString, &dataStart);

    // The starting index in the current graph for adding new vertices
    int startingIndex = g->numberOfVertices;

    // Set the neighbours to 0, two of the new vertices aren't getting added, we remove these
    for (int i = startingIndex; i < startingIndex + nbOfNewVertices - 2; i++) {
        g->verticesIndexed[i].neighbours = 0;
    }

    // First index as index the first indices are the number of vertices
    int index = dataStart;

    int bitPos = 0; // Bit position per index
    for (int i = 1; i < nbOfNewVertices; i++) {
        for (int j = 0; j < i; j++) {
            // Go through the entire adjacency matrix
            int part6bits = graphString[index] - 63;

            int bitShift = 5 - bitPos;
            // The amount you have to bitshift the number to check bitPos here:
            int edge = ((part6bits >> bitShift) & 1) == 1;
            if (edge) {

                /**
                We also need the vertices to come right after the original graph
                We keep track of an extra int

                If the index I or J have passed the indices we're linking, they should shift
                **/
                int addI = 0;
                if (i >= idxOneOwnGraph) addI -= 1;
                if (i >= idxTwoOwnGraph) addI -= 1;

                int addJ = 0;
                if (j >= idxOneOwnGraph) addJ -= 1;
                if (j >= idxTwoOwnGraph) addJ -= 1;


                // If i or j is the linked vertex, link the target instead
                vertex* vertex1 = &g->verticesIndexed[startingIndex + i + addI];
                vertex* vertex2 = &g->verticesIndexed[startingIndex + j + addJ];

                // I is one of the vertices that are supposed to be linked
                if (i == idxOneOwnGraph) {
                    vertex1 = targetA;
                } else if (i == idxTwoOwnGraph) {
                    vertex1 = targetB;
                }

                // J is one of the vertices that are supposed to be linked
                if (j == idxOneOwnGraph) {
                    vertex2 = targetA;
                } else if (j == idxTwoOwnGraph) {
                    vertex2 = targetB;
                }

                // We add the two vertices as neighbours
                addNeighbour(vertex1, vertex2);

            }

            bitPos++;
            if (bitPos == 6) {
                index++;
                bitPos = 0;
            }
        }
    }

    // We reset the graph to recolor
    resetGraph(g->numberOfVertices + nbOfNewVertices - 2);

}

// Helper to encode a 6-bit value into a printable graph6 character
char encode_val(int val) {
    return (char)(val + 63);
}


void to_graph6_large() {
    int n = g->numberOfVertices;

    // We start with encoding the number of vertices
    if (n <= 62) {
        printf("%c", encode_val(n));
    } else {
        // Prefix
        printf("~");
        // Break n into three 6-bit chunks
        printf("%c", encode_val((n >> 12) & 0x3F));
        printf("%c", encode_val((n >> 6) & 0x3F));
        printf("%c", encode_val(n & 0x3F));
    }

    int bit_buffer = 0;
    int bit_count = 0;

    for (int j = 1; j < n; j++) {
        for (int i = 0; i < j; i++) {
            bit_buffer <<= 1;
            if (g->verticesIndexed[i].neighbours & SHIFTL(j)) {
                bit_buffer |= 1;
            }
            bit_count++;

            // Pack 6 bits into one character
            if (bit_count == 6) {
                printf("%c", encode_val(bit_buffer));
                bit_buffer = 0;
                bit_count = 0;
            }
        }
    }

    // Padding with zeros on the right
    if (bit_count > 0) {
        bit_buffer <<= (6 - bit_count);
        printf("%c", encode_val(bit_buffer));
    }
    printf("\n");
}



