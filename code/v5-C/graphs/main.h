#ifndef MAIN_H
#define MAIN_H


int parseInt(char* str);
int parseBoolean(char* str);
void parseArguments(int argc, char **argv);
void printOverview(void);
int performComputation(int previousN, char line[]);
void freeGraph(graph* graph);
void printColors(int colors[]);

#endif //MAIN_H