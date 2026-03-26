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

// Global variables:
extern graph* g;

extern int minChrom;
extern int checkCondition;
extern int isOpenColoring;
extern int isProperColoring;
extern int isUMColoring;

extern int lengthOfGraph;

extern int (*handler)(int, vertex*, int, bitset_t, int);
extern int (*colorCheck)(vertex*, vertex*);
extern enum colorings coloring;

// Functions:

/**
 * Returns the final coloring of the current graph.
 * Calling this before actually coloring in the graph will return a nonsense answer.
 *
 * @param colors The colors array that will get filled with the colors.
 */
void getColors(int colors[]);

/**
 * Returns the number of vertices and updates dataStart so you know where the edge bits begin.
 *
 * @param graphString The graph6-string to find the number of vertices of.
 * @param dataStart The index in the graph6-string showing where the adjacency matrix starts.
 *                  This is needed for graphs with more than 62 vertices.
 *
 * @return The number of vertices.
 */
int getNumberOfVertices(char graphString[], int* dataStart);

/**
 * A helper function creating a graph.
 * Only when the previousN is 0 does this actually fully create a new graph.
 * Otherwise, it reuses the current graph g.
 *
 * @param previousN The number of vertices of the previously colored graph.
 * @param graphString The graph6-string of the graph that will get created.
 *
 * @return A pointer to the graph that was created (or reused).
 */
graph* createGraph(int previousN, char graphString[]);

/**
 * A function helping with resetting the current graph.
 * It resets the graph to work with the given number of vertices.
 *
 * @param n The number of vertices to make the reset graph have.
 */
void resetGraph(int n);

/**
 * The function setting the maximum color for the coloring of the current graph.
 * It also resets the graph for every max. color to try and color it in using the algorithm.
 *
 * @param startingColor The color to start with when coloring the graph.
 *                      This is useful when you only care about a certain chromatic number,
 *                      starting with that number - 1, will speed up the process, and still gets the results.
 * @param allColorings Whether all colorings of a graph should get found.
 *                     This isn't supported currently.
 *
 * @return One, if the chromatic number of the graph is found.
 *         Zero, otherwise.
 */
int findChromaticNumberOptimized(int startingColor, int allColorings);

/**
 * The main coloring algorithm responsible for finding a coloring of the current graph.
 * This is a recursive backtracking algorithm working by coloring in vertices one-by-one.
 * It tries all possible colors for a given vertex (kept track of by the vertex in question),
 * and updates the neighbors using updateNeighbors.
 *
 * @param maxColorCurrGraph The current maximum color in the graph.
 * @param maxColor The maximum color used when coloring in the graph.
 * @param index The current index of vertex that is getting colored in.
 * @param allColorings Whether all colorings of a graph should get found.
 *                     This isn't supported currently.
 * @param depth The depth of the graph in the changed 2D-array.
 *              Essentially symbolizing how many vertices have already been colored.
 *
 * @return One, if a coloring of this graph was found.
 *         Zero, otherwise.
 */
int optimizedAlgorithm(int maxColorCurrGraph, int maxColor, int index, int allColorings, int depth);

/**
 * The starting step in the coloring algorithm.
 * Mainly serves to support the checking of a condition.
 *
 * @param maxColor The maximum color used when coloring in the graph.
 *                 When finding the coloring, this is essentially the chromatic number.
 * @param allColorings Whether all colorings of a graph should get found.
 *                     This isn't supported currently.
 *
 * @return One, if the coloring process can stop.
 *         Zero, if more colorings of this graph should get found.
 */
int startingStep(int maxColor, int allColorings);

/**
 * Finds the best index of a vertex to color in the current graph.
 * This can only return an index from the available vertices list in the current graph.
 *
 * @return The best index in the graph.
 */
int getBestIndex();

/**
 * Updates the available colors of the neighbors according to the newly assigned color of a given vertex.
 *
 * @param v The vertex that was recently colored.
 * @param color The color assigned to the recently colored vertex.
 * @param depth The depth of the graph in the changed 2D-array.
 *              Essentially symbolizing how many vertices have already been colored.
 * @param maxColorInGraph The current maximum color in the graph.
 *
 * @return One, if removing the available colors allows for pruning the branch (the vertex doesn't have any colors left).
 *         Zero, otherwise.
 */
int updateNeighbors(vertex* v, int color, int depth, int maxColorInGraph);

/**
 * A function that adds back the removed colors to vertices.
 * In particular the colors saved in the changed list of the current graph.
 * Resets the changed entries on the given depth.
 *
 * @param depth The depth of the graph in the changed 2D-array.
 *              Essentially symbolizing how many vertices have already been colored.
 * @param maxColorInGraph The current maximum color in the graph.
 */
void addColorsBack(int depth, int maxColorInGraph);

/**
 * A handler for the case of Conflict-free colorings used in the update neighbors method.
 * Is used in the case of one neighbor left uncolored.
 * This is made to allow for ease of coding and not copying code.
 *
 * @return Zero, the coloring is always correct.
 */
int handleProper(int, vertex*, int, bitset_t, int);

/**
 * A handler for the case of Conflict-free colorings used in the update neighbors method.
 * Is used in the case of one neighbor left uncolored.
 *
 * @param depth The depth of the graph in the changed 2D-array.
 *              Essentially symbolizing how many vertices have already been colored.
 * @param toColorNeighbor The neighbor that still has to get colored.
 * @param toColorNeighborIndex The index of the to color neighbor.
 * @param neighborhood The neighborhood of the original vertex, a neighbor of the to color neighbor.
 * @param maxColorInGraph The current maximum color in the graph.
 *
 * @return One, if removing the available colors allows for pruning the branch (the vertex doesn't have any colors left).
 *         Zero, otherwise.
 */
int handleCF(int depth, vertex* toColorNeighbor, int toColorNeighborIndex, bitset_t neighborhood, int maxColorInGraph);

/**
 * A handler for the case of Unique maximum colorings used in the update neighbors method.
 * Is used in the case of one neighbor left uncolored.
 *
 * @param depth The depth of the graph in the changed 2D-array.
 *              Essentially symbolizing how many vertices have already been colored.
 * @param toColorNeighbor The neighbor that still has to get colored.
 * @param toColorNeighborIndex The index of the to color neighbor.
 * @param neighborhood The neighborhood of the original vertex, a neighbor of the to color neighbor.
 * @param maxColorInGraph The current maximum color in the graph.
 *
 * @return One, if removing the available colors allows for pruning the branch (the vertex doesn't have any colors left).
 *         Zero, otherwise.
 */
int handleUM(int depth, vertex* toColorNeighbor, int toColorNeighborIndex, bitset_t neighborhood, int maxColorInGraph);

/**
 * A handler for the case of odd colorings used in the update neighbors method.
 * Is used in the case of one neighbor left uncolored.
 *
 * @param depth The depth of the graph in the changed 2D-array.
 *              Essentially symbolizing how many vertices have already been colored.
 * @param toColorNeighbor The neighbor that still has to get colored.
 * @param toColorNeighborIndex The index of the to color neighbor.
 * @param neighborhood The neighborhood of the original vertex, a neighbor of the to color neighbor.
 * @param maxColorInGraph The current maximum color in the graph.
 *
 * @return One, if removing the available colors allows for pruning the branch (the vertex doesn't have any colors left).
 *         Zero, otherwise.
 */
int handleOdd(int depth, vertex* toColorNeighbor, int toColorNeighborIndex, bitset_t neighborhood, int maxColorInGraph);

/**
 * Removes the given available colors from a vertex. The color is given as a bitset.
 * This method also updates the changed list in the graph, while this wouldn't happen in vertex.c
 *
 * @param v The vertex to remove the available colors from.
 * @param index The index of the vertex.
 * @param color The color bitset to remove.
 * @param depth The depth of the graph in the changed 2D-array.
 *              Essentially symbolizing how many vertices have already been colored.
 * @param maxColorInGraph The current maximum color in the coloring of the graph.
 *
 * @return One, if the removing of the available colors allowed for pruning this branch (the vertex doesn't have any colors left).
 *         Zero, otherwise.
 */
int removeColorMask(vertex* v, int index, int color, int depth, int maxColorInGraph);

/**
 * Subdivide the graph so that each edge gets turned into a vertex and two edges.
 *
 * @param removeOriginalEdge Boolean saying whether the original edge
 *                           in the graph that is getting subdivided should get removed.
 */
void subdivide(int removeOriginalEdge);

/**
 * Add a given graph in graph6 format to the current graph g.
 * The graph gets linked by taking one vertex in the given graph
 * and putting that as a vertex in the current graph.
 *
 * @param graphString The graph to add to the current graph.
 * @param indexInThisGraph The vertex that will get the graph added to it.
 * @param indexInOwnGraph The vertex in the to add graph that will be used as the connector.
 */
void addGraphToIndex(char graphString[], int indexInThisGraph, int indexInOwnGraph);

/**
 * A method that replaces an edge in the current graph g with a given graph in graph6 format.
 *
 * @param graphString The graph that will be used to replace an edge.
 * @param idxOneThisGraph The first index of the edge to replace.
 * @param idxTwoThisGraph The second index of the edge to replace.
 * @param idxOneOwnGraph The first index of the vertex in the graph that is added.
 *                       This is a vertex that will be used as connection point between the two graphs.
 * @param idxTwoOwnGraph The second index of the vertex in the graph that is added.
 *                       This is a vertex that will be used as connection point between the two graphs.
 */
void replaceEdgeByGraph(char graphString[], int idxOneThisGraph, int idxTwoThisGraph, int idxOneOwnGraph, int idxTwoOwnGraph);

/**
 * Helper to encode a 6-bit value into a printable graph6 character.
 *
 * @param val The value to encode.
 * @return The encoded value.
 */
char encode_val(int val);

/**
 * Converts the current global graph into graph6 format and outputs it to stdout.
 */
void to_graph6();


#endif //GRAPH_H