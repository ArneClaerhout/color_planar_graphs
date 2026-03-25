#ifndef VERTEX_H
#define VERTEX_H

#include <stdint.h>
#include "colorings.h"
#include "types.h"


typedef struct vertex {
    bitset_t neighbors;
    int color;
    int availableColors;
    int amountOfAvailableColors;
    int index;
} vertex;


// Vertex functions
void addNeighbor(vertex* v, vertex* neighbor);
void removeNeighbor(vertex* v, vertex* neighbor);
void setMaxAvailableColors(vertex* v, int max);
// int removeColorFromAvailableColors(vertex* v, int color);
// int addColorFromAvailableColors(vertex* v, int color);
int isCorrectlyColoredOdd(vertex* v, vertex verticesIndexed[]);
int isCorrectlyColoredProper(vertex* v, vertex verticesIndexed[]);
int isCorrectlyColoredUM(vertex* v, vertex verticesIndexed[]);
int isCorrectlyColoredCF(vertex* v, vertex verticesIndexed[]);
int isCorrectlyColored(vertex* v, vertex* verticesIndexed[], enum colorings coloring);

#endif //VERTEX_H