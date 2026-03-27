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