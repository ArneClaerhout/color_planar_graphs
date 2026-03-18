#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

#include "counter.h"
#include "vertex.h"
#include "types.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

typedef struct graph {
    bitset_t availableVertices;
    bitset_t (*changed)[10];
    int chromaticNumber;
    bitset_t maxColoringMask;
    int numberOfVertices;
    counter* counter;
    vertex verticesIndexed[MAX_VERTICES];
} graph;

void getColors(int colors[]);
int getNumberOfVertices(char graphString[], int* dataStart);
graph* createGraph(int previousN, char graphString[]);
void resetGraph(int n);
int findChromaticNumberOptimized(int startingColor, int allColorings);
int optimizedAlgorithm(int maxColorCurrGraph, int maxColor, int index, int allColorings, int depth);
int startingStep(int maxColor, int allColorings);
int getBestIndex();
int updateNeighbours(vertex* v, int color, int depth, int maxColorInGraph);
void addColorsBack(int depth, int maxColorInGraph);
int handleProper(int, vertex*, int, bitset_t, int);
int handleCF(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, bitset_t neighbourhood, int maxColorInGraph);
int handleUM(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, bitset_t neighbourhood, int maxColorInGraph);
int handleOdd(int depth, vertex* toColorNeighbour, int toColorNeighbourIndex, bitset_t neighbourhood, int maxColorInGraph);
// int removeColor(vertex* v, int index, int color, int depth, int maxColorInGraph);
int removeColorMask(vertex* v, int index, int color, int depth, int maxColorInGraph);
void subdivide(int removeOriginalEdge);


#endif //GRAPH_H