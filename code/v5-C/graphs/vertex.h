#ifndef VERTEX_H
#define VERTEX_H

#include <stdint.h>
#include "vertex.h"
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

/**
 * Adds an edge between two given vertices.
 *
 * @param v The first vertex.
 * @param neighbor The second vertex.
 */
void addNeighbor(vertex* v, vertex* neighbor);

/**
 * Removes an edge between two given vertices.
 *
 * @param v The first vertex.
 * @param neighbor The second vertex.
 */
void removeNeighbor(vertex* v, vertex* neighbor);

/**
 * Sets the available colors of a given vertex to include the colors less than or equal to the given max.
 *
 * @param v The vertex to set the available colors of.
 * @param max The max color to add.
 */
void setMaxAvailableColors(vertex* v, int max);

/**
 * Checks whether a given vertex is correctly colored according to the coloring rules of the odd coloring.
 *
 * @param v The vertex to check.
 * @param verticesIndexed The vertex array of the graph.
 * @return One, if the vertex is correctly colored.
 *         Zero, otherwise.
 */
int isCorrectlyColoredOdd(vertex* v, vertex verticesIndexed[]);

/**
 * Checks whether a given vertex is correctly colored according to the coloring rules of the proper coloring.
 * This is only called after the checks have already happened. This is therefore only used so no code is repeated.
 *
 * @return 1.
 */
int isCorrectlyColoredProper(vertex* v, vertex verticesIndexed[]);

/**
 * Checks whether a given vertex is correctly colored according to the coloring rules of the Unique-maximum coloring.
 *
 * @param v The vertex to check.
 * @param verticesIndexed The vertex array of the graph.
 * @return One, if the vertex is correctly colored.
 *         Zero, otherwise.
 */
int isCorrectlyColoredUM(vertex* v, vertex verticesIndexed[]);

/**
 * Checks whether a given vertex is correctly colored according to the coloring rules of the Conflict-free coloring.
 *
 * @param v The vertex to check.
 * @param verticesIndexed The vertex array of the graph.
 * @return One, if the vertex is correctly colored.
 *         Zero, otherwise.
 */
int isCorrectlyColoredCF(vertex* v, vertex verticesIndexed[]);

/**
 * Checks whether a given vertex is correctly colored according to the coloring rules of the global coloring variable.
 *
 * @param v The vertex to check.
 * @param verticesIndexed The vertex array of the graph.
 * @return One, if the vertex is correctly colored.
 *         Zero, otherwise.
 */
int isCorrectlyColored(vertex* v, vertex* verticesIndexed[]);

#endif //VERTEX_H