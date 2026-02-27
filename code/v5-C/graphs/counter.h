#ifndef COUNTER_H
#define COUNTER_H
#include <stdint.h>

typedef struct counter {
    int firstInput;
    uint64_t condition[10];
    uint64_t conditionVertices;
    int numberOfVertices;
    uint64_t maxColoringMask;
    int checkAlwaysColor; // If false = checkNeverColor
    int* allColorings[];
} counter;

int inputColors(counter* counter, const int colors[], int allColors, int chromaticNumber);
int isConditionMet(counter* counter, int chromaticNumber);
void getColoringAfterCheck(counter* counter, int chromaticNumber, int colors[]);

#endif //COUNTER_H