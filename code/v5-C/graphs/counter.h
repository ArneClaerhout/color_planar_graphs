#ifndef COUNTER_H
#define COUNTER_H
#include <stdint.h>
#include "types.h"


#define MAX_STRING_LENGTH 40

typedef struct counter {
    int firstInput;
    bitset_t condition[MAX_VERTICES];
    bitset_t conditionVertices;
    int numberOfVertices;
    char extraInfo[MAX_STRING_LENGTH];
    bitset_t maxColoringMask;
} counter;

void startCounter(int n);
int inputColors(counter* counter, const int colors[], int allColors, int chromaticNumber);
int isConditionMet(counter* counter, int chromaticNumber);
void getColoringAfterCheck(counter* counter, int chromaticNumber, int colors[]);
char* getExtraInfoText(counter* counter);

#endif //COUNTER_H