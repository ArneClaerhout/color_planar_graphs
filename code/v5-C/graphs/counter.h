#ifndef COUNTER_H
#define COUNTER_H
#include <stdint.h>

#define MAX_STRING_LENGTH 40

typedef struct counter {
    int firstInput;
    uint64_t condition[10];
    uint64_t conditionVertices;
    int numberOfVertices;
    char extraInfo[MAX_STRING_LENGTH];
    uint64_t maxColoringMask;
} counter;

void startCounter(int n);
int inputColors(counter* counter, const int colors[], int allColors, int chromaticNumber);
int isConditionMet(counter* counter, int chromaticNumber);
void getColoringAfterCheck(counter* counter, int chromaticNumber, int colors[]);
char* getExtraInfoText(counter* counter);

#endif //COUNTER_H