#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

#include "counter.h"
#include "vertex.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define MAX_VERTICES 64

typedef struct graph {
    uint64_t vertexIsColored;
    uint64_t availableVertices;
    uint64_t (*changed)[10];
    int chromaticNumber;
    uint64_t maxColoringMask;
    int numberOfVertices;
    counter* counter;
    vertex verticesIndexed[MAX_VERTICES];
} graph;

void getColors(int colors[]);
int getNumberOfVertices(char graphString[]);
graph* createGraph(int previousN, char graphString[]);
void resetGraph(int n);
int findChromaticNumberOptimized(int startingColor, int allColorings);
int optimizedAlgorithm(int maxColorCurrGraph, int maxColor, int index, int allColorings, int depth);
int startingStep(int maxColor, int allColorings);
int getBestIndex(int indexColored);
int updateNeighbours(vertex* v, int color, int depth, int maxColorInGraph);
void addColorsBack(int depth, int maxColorInGraph);
int handleProper(int, vertex*, int, uint64_t, int);
int handleCF(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood, int maxColorInGraph);
int handleUM(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood, int maxColorInGraph);
int handleOdd(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, uint64_t neighbourhood, int maxColorInGraph);
int removeColor(vertex* v, int index, int color, int depth, int maxColorInGraph);
int removeColorMask(vertex* v, int index, int color, int depth, int maxColorInGraph);
void subdivide(int removeOriginalEdge);


#endif //GRAPH_H