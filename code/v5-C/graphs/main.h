#ifndef MAIN_H
#define MAIN_H

#define SIZE(array) (sizeof(array)/sizeof(array[0]))

int parseInt(char* str);
int parseBoolean(char* str);
void parseArguments(int argc, char **argv);
void printOverview(void);
int performComputation(int previousN, char line[]);
void freeGraph(graph* graph);

#endif //MAIN_H