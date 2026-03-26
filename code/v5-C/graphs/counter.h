#ifndef COUNTER_H
#define COUNTER_H
#include <stdint.h>
#include "types.h"
#include "vertex.h"


#define MAX_STRING_LENGTH 65

typedef struct counter {
    int firstInput;
    bitset_t condition[MAX_VERTICES];
    bitset_t conditionVertices;
    int numberOfVertices;
    char extraInfo[MAX_STRING_LENGTH];
    bitset_t maxColoringMask;
} counter;

/**
 * Starts up a counter for the current graph.
 * If a counter already exists, it gets reused.
 *
 * @param n The number of vertices this counter will keep track of.
 */
void startCounter(int n);

/**
 * Inputs the given coloring into the given counter.
 *
 * @param counter The counter to input the coloring into.
 * @param colors The coloring of a graph to input.
 * @param allColors Whether all colorings should get input. Currently not supported.
 * @param chromaticNumber The chromatic number of the graph.
 * @return One, if the inputting of colorings can stop (because the condition isn't met).
 *         Zero, otherwise.
 */
int inputColors(counter* counter, const int colors[], int allColors, int chromaticNumber);

/**
 * Checks whether the condition has been met after inputting all the colorings into counter.
 *
 * @param counter The counter used to check for the condition.
 * @param chromaticNumber The chromatic number of the graph.
 * @return One, if the condition is met.
 *         Zero, otherwise.
 */
int isConditionMet(counter* counter, int chromaticNumber);

/**
 * Finds the coloring after all possible colorings of the graph have been input into the counter.
 * This will set the given colors array to the coloring.
 * For most colorings, this will not return an actual coloring of the graph,
 * only a representation of what vertices were important in the checking of a condition.
 *
 * @param counter The counter that was used to input all colorings.
 * @param chromaticNumber The chromatic number of the graph.
 * @param colors The colors array used to write the coloring into.
 */
void getColoringAfterCheck(counter* counter, int chromaticNumber, int colors[]);

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

#endif //COUNTER_H