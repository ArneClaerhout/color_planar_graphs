#include "counter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vertex.h"
#include "graph.h"


int colorCheckiCFc(vertex* v, vertex verticesIndexed[]) {
    if (v == &verticesIndexed[index1iCFc] || v == &verticesIndexed[index2iCFc]) {
        return 1;
    }
    return isCorrectlyColoredCF(v, verticesIndexed);
}


char* getExtraInfoText(counter* counter) {
    return counter->extraInfo;
}