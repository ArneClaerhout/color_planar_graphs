#ifndef VERTEX_H
#define VERTEX_H

#include <stdint.h>
#include "vertex.h"
#include "colorings.h"
#include "types.h"


typedef struct vertex {
    bitset_t neighbours;
    int color;
    int availableColors;
    int amountOfAvailableColors;
    int index;
} vertex;


// Vertex functions
void addNeighbour(vertex* v, vertex* neighbour);
void removeNeighbour(vertex* v, vertex* neighbour);
void setMaxAvailableColors(vertex* v, int max);
// int removeColorFromAvailableColors(vertex* v, int color);
// int addColorFromAvailableColors(vertex* v, int color);
int isCorrectlyColoredOdd(vertex* v, vertex verticesIndexed[]);
int isCorrectlyColoredProper(vertex* v, vertex verticesIndexed[]);
int isCorrectlyColoredUM(vertex* v, vertex verticesIndexed[]);
int isCorrectlyColoredCF(vertex* v, vertex verticesIndexed[]);
int isCorrectlyColored(vertex* v, vertex* verticesIndexed[], enum colorings coloring);

#endif //VERTEX_H