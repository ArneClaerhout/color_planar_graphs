#ifndef MAIN_H
#define MAIN_H

#define SIZE(array) (sizeof(array)/sizeof(array[0]))

int parseInt(char* str);
int parseBoolean(char* str);
void parseArguments(int argc, char **argv);
void printOverview(void);
void performComputation(char line[]);

#endif //MAIN_H