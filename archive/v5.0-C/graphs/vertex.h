#ifndef VERTEX_H
#define VERTEX_H

#include <stdint.h>
#include "vertex.h"
#include "colorings.h"


#define SHIFT(n) (1 << n)
#define SHIFTL(n) (1L << n)

#define FOR_EACH_BIT(i, mask) \
for (uint64_t _m = (mask); _m; _m &= _m - 1) \
for (int i = __builtin_ctzll(_m), _once = 1; _once; _once = 0)


typedef struct vertex {
    uint64_t neighbours;
    int color;
    int availableColors;
    int amountOfAvailableColors;
    int index;
} vertex;

// Vertex functions
void addNeighbour(vertex* v, vertex* neighbour);
void removeNeighbour(vertex* v, vertex* neighbour);
void setMaxAvailableColors(vertex* v, int max);
int removeColorFromAvailableColors(vertex* v, int color);
int addColorFromAvailableColors(vertex* v, int color);
int isCorrectlyColoredOdd(vertex* v, vertex verticesIndexed[], int fillUncolored);
int isCorrectlyColoredProper(vertex* v, vertex verticesIndexed[], int fillUncolored);
int isCorrectlyColoredUM(vertex* v, vertex verticesIndexed[], int fillUncolored);
int isCorrectlyColoredCF(vertex* v, vertex verticesIndexed[], int fillUncolored);
int isCorrectlyColored(vertex* v, vertex* verticesIndexed[], enum colorings coloring, int fillUncolored);

#endif //VERTEX_H