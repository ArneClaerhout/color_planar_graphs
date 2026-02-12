#ifndef V5_C_GRAPH_H
#define V5_C_GRAPH_H

#include <stdint.h>

#include "counter.h"
#include "vertex.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct graph {
    uint64_t vertexIsColored;
    uint64_t availableVertices;
    int chromaticNumber;
    uint64_t maxColoringMask;
    int numberOfVertices;
    counter* counter;
    vertex* verticesIndexed[];
} graph;

void getColors(graph* g, int colors[]);
int getNumberOfVertices(char graphString[]);
graph* createGraph(char graphString[]);
int findChromaticNumberOptimized(graph* g, enum colorings coloring, int startingColor, int allColorings);
int optimizedAlgorithm(graph* g, enum colorings coloring, int maxColorCurrGraph, int maxColor, int index, int allColorings);
int startingStep(graph* g, int maxColor, int allColorings);
int getBestIndex(graph* g, int indexColored);
int updateNeighbours(graph* g, vertex* v, int color, enum colorings coloring, uint64_t changed[]);
void addColorsBack(graph* g, const uint64_t changed[]);
int handleCF(graph* g, uint64_t changed[], vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood);
int handleUM(graph* g, uint64_t changed[], vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood);
int handleOdd(graph* g, uint64_t changed[], vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood);
int removeColor(graph* g, vertex* v, int index, int color, uint64_t changed[]);

#endif //V5_C_GRAPH_H