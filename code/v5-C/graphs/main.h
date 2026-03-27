#ifndef MAIN_H
#define MAIN_H

/**
 * Parses a integer string into an integer.
 *
 * @param str The string to parse.
 * @return The parsed int.
 */
int parseInt(char* str);

/**
 * Parses a boolean string into a number.
 *
 * @param str The string to parse.
 * @return The parsed int (1 or 0).
 */
int parseBoolean(char* str);

/**
 * Parses the arguments given in the main of the program.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 */
void parseArguments(int argc, char **argv);

/**
 * Prints the data found in the cNumbers map,
 * essentially provides an overview of the chromatic number of all the colored in graphs.
 */
void printOverview(void);

/**
 * Performs the computation of a given graph.
 * This will color in the graph, check if it has conditions if needed.
 * Lastly, it will also print out the graph in the correct format (given by raw and overview).
 *
 * @param g The graph.
 * @param line The graph6-string to process.
 *
 * @return The graph object used in the computation.
 */
graph* performComputation(graph* g, char line[]);

/**
 * Frees a given graph from memory.
 *
 * @param graph The graph to free.
 */
void freeGraph(graph* graph);

/**
 * Prints out a given colors array to stdout.
 *
 * @param g The graph.
 * @param colors The colors array to print out.
 */
void printColors(graph* g, int colors[]);

#endif //MAIN_H