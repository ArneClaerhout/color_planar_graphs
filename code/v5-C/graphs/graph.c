#include "colorings.h"
#include "graph.h"

#include <stdatomic.h>

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
    // 1. ATOMIC READ: Check if we should stop
    int localFound;
    #pragma omp atomic read
    localFound = found;
    if (localFound || g->availableVertices == 0) {
        if (g->availableVertices == 0 && startingStep(g, maxColor)) {
            #pragma omp atomic write
            found = 1;
        }
        return;
    }

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

        graph* branchGraph = copyGraph(g);
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

int startParallelColoring(graph* originalG, int maxColor) {
    found = 0;


    #pragma omp parallel
    {
        // fprintf(stderr, "Thread %d of %d\n", omp_get_thread_num(), omp_get_num_threads());
        #pragma omp single
        {
            // We create one initial copy for the root of the parallel search
            graph* rootCopy = copyGraph(originalG);

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




void startCounter(graph* g, int n) {
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


int inputColors(graph* g, const int colors[], int chromaticNumber) {
    counter* counter = g->counter;
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
            subdivide(g, checkCondition != 1);
            // We fully reset the graph
            resetGraph(g, g->numberOfVertices);

            // We briefly change the coloring
            coloring = iCFo; isProperColoring = 0; isOpenColoring = 1; isUMColoring = 0;

            int c = findChromaticNumberOptimized(g, 4);
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


int isConditionMet(graph* g, int chromaticNumber) {
    counter* counter = g->counter;
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
    counter* counter = g->counter;
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
        getColors(g, colors);
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




