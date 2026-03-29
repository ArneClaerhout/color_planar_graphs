//
// Created by arne3 on 29/03/2026.
//

#ifndef COUNTERINPUT_H
#define COUNTERINPUT_H
#include "counter.h"
#include "vertex.h"
#include "graph.h"

extern int index1iCFc;
extern int index2iCFc;

/**
 * Helper method for finding the extra info of the counter.
 * The coloring should have already been asked for.
 *
 * @param counter The counter to get the extra info of.
 * @return The extra info.
 */
char* getExtraInfoText(counter* counter);

/**
 * A helper function used to alternatively color in a graph using this special color checker method.
 *
 * @param v The vertex that is going to get checked for correctness of coloring.
 * @param verticesIndexed The array of vertices in the current graph.
 * @return One, if the vertex is correctly colored.
 *         Zero, otherwise.
 */
int colorCheckiCFc(vertex* v, vertex verticesIndexed[]);

/**
 * Starts up a counter for the current graph.
 * If a counter already exists, it gets reused.
 *
 * @param g The graph.
 * @param n The number of vertices this counter will keep track of.
 */
void startCounter(graph* g, int n);

/**
 * Inputs the given coloring into the given counter.
 *
 * @param g The graph.
 * @param colors The coloring of a graph to input.
 * @param chromaticNumber The chromatic number of the graph.
 * @return One, if the inputting of colorings can stop (because the condition isn't met).
 *         Zero, otherwise.
 */
int inputColors(graph* g, const int colors[], int chromaticNumber);

/**
 * Checks whether the condition has been met after inputting all the colorings into counter.
 *
 * @param g The graph.
 * @param chromaticNumber The chromatic number of the graph.
 * @return One, if the condition is met.
 *         Zero, otherwise.
 */
int isConditionMet(graph* g, int chromaticNumber);

/**
 * Finds the coloring after all possible colorings of the graph have been input into the counter.
 * This will set the given colors array to the coloring.
 * For most colorings, this will not return an actual coloring of the graph,
 * only a representation of what vertices were important in the checking of a condition.
 *
 * @param g The graph.
 * @param chromaticNumber The chromatic number of the graph.
 * @param colors The colors array used to write the coloring into.
 */
void getColoringAfterCheck(graph* g, int chromaticNumber, int colors[]);

#endif //COUNTERINPUT_H
