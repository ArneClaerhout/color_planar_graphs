#include "colorings.h"
#include "graph.h"
#include "counterInput.h"
#include "vertex.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <omp.h>

#include "main.h"

int index1iCFc;
int index2iCFc;

int found = 0;

void getColors(graph* g, int colors[]) {
    for (int i = 0; i < g->numberOfVertices; i++) {
        colors[i] = g->verticesIndexed[i].color;
    }
}


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


graph* createGraph(graph* g, char graphString[]) {

    int dataStart;
    int n = getNumberOfVertices(graphString, &dataStart);

    if (g == NULL) {
        // There wasn't a graph before, we create one
        g = (graph*) malloc(sizeof(graph));
        // We immediately set the counter to zero, we can't have any freeing problems later
        g->counter = 0;
        g->changed = malloc(sizeof(bitset_t[MAX_VERTICES][10]));
        for (int i = 0; i < MAX_VERTICES; i++) {
            g->verticesIndexed[i].index = i;
        }
    }

    resetGraph(g, n);

    for (int i = 0; i < n; i++) {
        g->verticesIndexed[i].neighbors = 0;
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
                addNeighbor(&g->verticesIndexed[i], &g->verticesIndexed[j]);
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

graph* copyGraph(graph* g) {
    graph* copy = malloc(sizeof(graph));
    if (!copy) return NULL;

    memcpy(copy, g, sizeof(graph));
    // We also set a different changed
    copy->changed = calloc(MAX_VERTICES, sizeof(bitset_t[10]));
    if (!copy->changed) {
        free(copy); // Clean up the struct if the second allocation fails
        return NULL;
    }

    return copy;
}


void resetGraph(graph* graph, int n) {
    graph->numberOfVertices = n;
    graph->maxColoringMask = SHIFTL(graph->numberOfVertices) - 1;
    graph->chromaticNumber = 0;

    // Set the changed 2D-array to zeroes
    memset(graph->changed,0, sizeof(bitset_t[n][10]));

    startCounter(graph, n);

    // for (int i = 0; i < n; i++) {
    //     g->verticesIndexed[i].color = 0;
    // }
}


int findChromaticNumberOptimized(graph* g, int startingColor) {
    for (int i = startingColor; i <= 10; i++) {
        // fprintf(stderr, "%d\n", i);
        for (int j = 0; j < g->numberOfVertices; j++) {
            setMaxAvailableColors(&g->verticesIndexed[j], i);
        }
        // Each time we reset which vertices are colored.
        g->availableVertices = g->maxColoringMask;

        // Color in the graph
        if (g->numberOfVertices > 63) {
            // If the graph has a lot of vertices, use multithreading
            if (startParallelColoring(g, i)) {
                g->chromaticNumber = i;
                return i;
            }
        } else {
            // Otherwise, color it in normally
            if (optimizedAlgorithm(g, 0, i, 0, 0)) {
                g->chromaticNumber = i;
                return i;
            }
        }

        if (checkCondition != 0 && g->chromaticNumber != 0) {
            g->chromaticNumber = i;
            return i; // we wanted to find all colorings
        }
    }
    return 0;
}

void parallelWorker(graph* g, int maxColorCurrGraph, int maxColor, int index, int depth) {
    // We work with a found variable
    int localFound;
    #pragma omp atomic read
    localFound = found;

    // If there was a thread that found a coloring, return
    // If the entire graph was colored in, return
    if (localFound || g->availableVertices == 0) {
        if (g->availableVertices == 0 && startingStep(g, maxColor)) {
            #pragma omp atomic write
            found = 1;
        }
        return;
    }

    // Start coloring in
    vertex* v = &g->verticesIndexed[index];
    int colors = v->availableColors;
    int maxLoop = (isUMColoring || checkCondition != 0) ? maxColor : min(maxColorCurrGraph + 1, maxColor);
    colors &= SHIFT(maxLoop) - 1;

    g->availableVertices &= ~(1L << index);

    FOR_EACH_BIT(color, colors) {

        // Check flag inside the loop to break early
        #pragma omp atomic read
        localFound = found;
        if (localFound) break;

        // Make a new graph for each branch
        graph* branchGraph = copyGraph(g);
        // Color in the copied graph
        vertex* branchV = &branchGraph->verticesIndexed[index];
        branchV->color = color + 1;
        int nextMax = max(color + 1, maxColorCurrGraph);

        if (updateNeighbors(branchGraph, branchV, color, depth, nextMax)) {
            freeGraph(branchGraph);
            continue;
        }

        int nextIndex = getBestIndex(branchGraph);

        if (depth < 4) {
            // Task creation for high-level branches
            #pragma omp task firstprivate(branchGraph, nextMax, nextIndex, depth) shared(found)
            {
                parallelWorker(branchGraph, nextMax, maxColor, nextIndex, depth + 1);
                freeGraph(branchGraph); // Clean up the copy inside the task
            }
        } else {
            // Serial execution for deep branches
            if (optimizedAlgorithm(branchGraph, nextMax, maxColor, nextIndex, depth + 1)) {
                #pragma omp atomic write
                found = 1;
            }
            freeGraph(branchGraph);
        }
    }
}

int startParallelColoring(graph* g, int maxColor) {
    found = 0;
    #pragma omp parallel
    {
        // fprintf(stderr, "Thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
        #pragma omp single
        {
            // We create one initial copy for the root of the parallel search
            graph* rootCopy = copyGraph(g);

            parallelWorker(rootCopy, 0, maxColor, 0, 0);

            // This ensures all tasks finish before we exit the parallel region
            #pragma omp taskwait
            freeGraph(rootCopy);
        }
    }

    return found;
}

int optimizedAlgorithm(graph* g, int maxColorCurrGraph, int maxColor, int index, int depth) {

    if (found) return 0;

    if (g->availableVertices == 0) {
        return startingStep(g, maxColor);
    }

    vertex* v = &g->verticesIndexed[index];
    int colors = v->availableColors;

    // Every coloring should be tried for um, as this is different then other colorings
    int maxLoop = (isUMColoring || checkCondition != 0) ? maxColor : min(maxColorCurrGraph + 1, maxColor);

    // We remove the colors that aren't possible (those higher than maxLoop)
    colors &= SHIFT(maxLoop) - 1;

    // This vertex isn't available anymore
    g->availableVertices &= ~(SHIFTL(index));

    FOR_EACH_BIT(color, colors) {

        v->color = color + 1; // + 1 as the actual colors are from 1 to n

        int newMaxColorCurrGraph = max(maxColorCurrGraph, color + 1);

        /**
        There is no check here for when all neighbors are colored.
        This is made sure by updateNeighbors of previous iterations
        **/

        // We also change the available colors for the neighbors
        if (updateNeighbors(g, v, color, depth, newMaxColorCurrGraph)) {
            // This is used to skip this color, as it isn't possible
            continue;
        }

        int newIndex = getBestIndex(g);
        if (optimizedAlgorithm(g, newMaxColorCurrGraph, maxColor, newIndex, depth + 1)) {
            return 1;
        }

        // We add back the available colors if it didn't work out
        addColorsBack(g, depth, newMaxColorCurrGraph);
    }

    /**
    No Coloring was found.
    We decolor this vertex.
    **/
    // v->color = 0; // This actually doesn't matter
    g->availableVertices |= (1L << index);

    return 0;
}


int startingStep(graph* g, int maxColor) {
    if (checkCondition != 0) {
        if (maxColor < minChrom) {
            // When it's going to get filtered away anyway, we stop it
            return 1;
        }
        g->chromaticNumber = maxColor;
        // If the counters are all full, return true and stop counting
        // Otherwise, this will return false and we keep trying
        int colors[g->numberOfVertices];
        getColors(g, colors);
        return inputColors(g, colors, g->chromaticNumber);
    }
    return 1;
}


int getBestIndex(graph* g) {
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



int updateNeighbors(graph* g, vertex* v, int color, int depth, int maxColorInGraph) {
    FOR_EACH_BIT(bit, v->neighbors) {
        vertex* neighbor = &g->verticesIndexed[bit];

        int neighborIsColored = (SHIFTL(bit) & g->availableVertices) == 0;

        // Neighboring vertices can't have the same color in proper colorings:
        if (isProperColoring && !neighborIsColored) {
            if (removeColorMask(g, neighbor, neighbor->index, SHIFT(color), depth, maxColorInGraph))
                return 1;
        }

        bitset_t neighborhood = neighbor->neighbors;
        neighborhood = (isOpenColoring ? neighborhood : (neighborhood | SHIFTL(bit)));
        bitset_t diff = neighborhood & g->availableVertices;

        if (diff == 0) {
            // All the neighbor's neighbors are colored and the neighbor itself is colored
            // We want to check if the neighbor is CORRECTLY colored
            if (!isOpenColoring && !colorCheck(neighbor, g->verticesIndexed)) {
                // Early pruning
                addColorsBack(g, depth, maxColorInGraph);
                return 1;
                // We skip the rest, as this color is incorrect
            }
        } else if ((diff & (diff - 1)) == 0) { // bitCount(diff) == 1
            // The one neighbor we still have to color:
            int toColorNeighborIndex = bitset_ctz(diff);

            // Only when changing available colors leads to a state
            // where a coloring is not possible do we actually return 1.
            if (handler(g, depth, toColorNeighborIndex, neighborhood, maxColorInGraph)) {
                return 1;
            }
        }
    }

    return 0;
}



void addColorsBack(graph* g, int depth, int maxColorInGraph) {
    for (int i = 0; i < maxColorInGraph; i++) {
        bitset_t value = g->changed[depth][i];
        if (value != 0) {
            FOR_EACH_BIT(index, value) {
                vertex* changedNeighbor = &g->verticesIndexed[index];
                // addColorFromAvailableColors(changedNeighbor, i);
                // We know for sure that this was removed,
                // we don't have to do the check performed in the addColorFromAvailableColors method
                changedNeighbor->availableColors |= SHIFT(i);
                changedNeighbor->amountOfAvailableColors++;
            }
            g->changed[depth][i] = 0; // We reset changed
        }
    }
}


int handleProper(graph*, int, int, bitset_t, int) {
    return 0;
}


int handleCF(graph* g, int depth, int toColorNeighborIndex, bitset_t neighborhood, int maxColorInGraph) {

    vertex* toColorNeighbor = &g->verticesIndexed[toColorNeighborIndex];

    int colorsOccurOnce = 0;
    int colorsOccur = 0;

    neighborhood = neighborhood & ~SHIFTL(toColorNeighborIndex);
    FOR_EACH_BIT(index, neighborhood) {
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
        return removeColorMask(g, toColorNeighbor, toColorNeighborIndex, colorsOccur, depth, maxColorInGraph);
    } else if ((colorsOccurOnce & (colorsOccurOnce - 1)) == 0) { // bitCount == 1
        return removeColorMask(g, toColorNeighbor, toColorNeighborIndex, colorsOccurOnce, depth, maxColorInGraph);
    }
    return 0;
}


int handleUM(graph* g, int depth, int toColorNeighborIndex, bitset_t neighborhood, int maxColorInGraph) {

    vertex* toColorNeighbor = &g->verticesIndexed[toColorNeighborIndex];

    int max = 1;
    int amountOfMax = 0;

    neighborhood = neighborhood & ~SHIFTL(toColorNeighborIndex);
    FOR_EACH_BIT(index, neighborhood) {
        int neighborColor = g->verticesIndexed[index].color;

        if (neighborColor == max) {
            amountOfMax++;
        } else if (neighborColor > max) {
            max = neighborColor;
            amountOfMax = 1;
        }
    }

    // We need to find the maximum and the amount of times it occurs
    // If it occurs once, don't use this color, if it occurs more than once
    // Only allow colors bigger than the max
    // Otherwise, do nothing.
    if (amountOfMax == 1) {
        return removeColorMask(g, toColorNeighbor, toColorNeighborIndex, SHIFT((max - 1)), depth, maxColorInGraph);
    }
    // The number of max is greater than 1 (because it can't be 0, there is always a max)
    return removeColorMask(g, toColorNeighbor, toColorNeighborIndex, SHIFT(max) - 1, depth, maxColorInGraph);
}


int handleOdd(graph* g, int depth, int toColorNeighborIndex, bitset_t neighborhood, int maxColorInGraph) {

    vertex* toColorNeighbor = &g->verticesIndexed[toColorNeighborIndex];

    int colorsOccurOdd = 0;

    neighborhood = neighborhood & ~SHIFTL(toColorNeighborIndex);
    FOR_EACH_BIT(index, neighborhood) {
        vertex *secondNeighbor = &g->verticesIndexed[index];
        int neighborColor = secondNeighbor->color;

        colorsOccurOdd ^= SHIFT((neighborColor - 1));
        // We do -1 as the colors are from 1...k,
        // but we want to later on use the colors 0...k-1
    }

    if (colorsOccurOdd != 0 && (colorsOccurOdd & (colorsOccurOdd - 1)) == 0) { // Check whether there's one bit
        // Don't take this color
        return removeColorMask(g, toColorNeighbor, toColorNeighborIndex, colorsOccurOdd, depth, maxColorInGraph);
    }
    return 0;
}


int removeColorMask(graph* g, vertex* v, int index, int color, int depth, int maxColorInGraph) {
    int changedColors = v->availableColors & color;
    if (changedColors == 0) {
        return 0;
    }
    if (v->availableColors == changedColors) {
        // It will remove all available colors
        addColorsBack(g, depth, maxColorInGraph);
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


void subdivide(graph* g, int removeOriginalEdge) {
    // We also reset the graph during this process
    int count = g->numberOfVertices;
    int index = count; // Keeps track of where to add the newest subdivision
    for (int i = 0; i < g->numberOfVertices; i++) {
        vertex* v = &g->verticesIndexed[i];
        v->color=0;
        FOR_EACH_BIT(j, v->neighbors & (SHIFTL(i) - 1)) {
            vertex* neighbor = &g->verticesIndexed[j];
            // Make the new neighbor
            vertex* newNeighbor = &g->verticesIndexed[count];
            newNeighbor->index = count;
            newNeighbor->neighbors = 0;
            newNeighbor->color = 0;

            if (removeOriginalEdge) {
                removeNeighbor(v, neighbor);
            }
            addNeighbor(v, newNeighbor);
            addNeighbor(neighbor, newNeighbor);
            count++;
        }
    }
    g->numberOfVertices = count; // We update the number of vertices
    g->maxColoringMask = SHIFTL(g->numberOfVertices) - 1;
    g->availableVertices = g->maxColoringMask;
    startCounter(g, count);
}



void addGraphToIndex(graph* g, char graphString[], int indexInThisGraph, int indexInOwnGraph) {

    // Grab the number of vertices of the to add graph
    int dataStart;
    int nbOfNewVertices = getNumberOfVertices(graphString, &dataStart);

    // The intermediate vertex
    vertex* target = &g->verticesIndexed[indexInThisGraph];
    // The starting index in the current graph for adding new vertices
    int startingIndex = g->numberOfVertices;

    // Set the neighbors to 0
    for (int i = startingIndex; i < startingIndex + nbOfNewVertices - 1; i++) {
        g->verticesIndexed[i].neighbors = 0;
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

                // We add the two vertices as neighbors
                addNeighbor(vertex1, vertex2);
            }

            bitPos++;
            if (bitPos == 6) {
                index++;
                bitPos = 0;
            }
        }
    }

    // We reset the graph to recolor
    resetGraph(g, g->numberOfVertices + nbOfNewVertices - 1);
}



void replaceEdgeByGraph(graph* g, char graphString[], int idxOneThisGraph, int idxTwoThisGraph, int idxOneOwnGraph, int idxTwoOwnGraph) {
    // The intermediate vertices
    vertex* targetA = &g->verticesIndexed[idxOneThisGraph];
    vertex* targetB = &g->verticesIndexed[idxTwoThisGraph];

    // Firstly, we remove the edge between the two vertices in this graph
    removeNeighbor(targetA, targetB);

    /**
    Now it's time to add the graph to the current graph:
    **/

    // Grab the number of vertices of the graph we're adding
    int dataStart;
    int nbOfNewVertices = getNumberOfVertices(graphString, &dataStart);

    // The starting index in the current graph for adding new vertices
    int startingIndex = g->numberOfVertices;

    // Set the neighbors to 0, two of the new vertices aren't getting added, we remove these
    for (int i = startingIndex; i < startingIndex + nbOfNewVertices - 2; i++) {
        g->verticesIndexed[i].neighbors = 0;
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

                // We add the two vertices as neighbors
                addNeighbor(vertex1, vertex2);

            }

            bitPos++;
            if (bitPos == 6) {
                index++;
                bitPos = 0;
            }
        }
    }

    // We reset the graph to recolor
    resetGraph(g, g->numberOfVertices + nbOfNewVertices - 2);

}


char encode_val(int val) {
    return (char)(val + 63);
}


void to_graph6(graph* g) {
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
            if (g->verticesIndexed[i].neighbors & SHIFTL(j)) {
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





