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

#endif //COUNTER_H