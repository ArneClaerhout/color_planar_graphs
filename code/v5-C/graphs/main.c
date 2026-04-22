#include "colorings.h"
#include "vertex.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "gadget_finder.h"
#include "gadget_finder_input.h"
#include "types.h"


enum colorings coloring;
int overview;
int cNumbers[10];
int minChrom;
int raw;
int checkCondition;
int isOpenColoring;
int isProperColoring;
int isUMColoring;
int doSubdivide;

int lengthOfGraph;

int (*handler)(graph*, int, int, bitset_t, int);
int (*colorCheck)(vertex*, vertex*);


uint64_t start;
uint64_t end;

int main(int argc, char **argv) {


    // First: parse the arguments
    parseArguments(argc, argv);

    isOpenColoring = coloringIsOpen(coloring);
    isProperColoring = coloringIsProper(coloring);
    isUMColoring = coloringIsUM(coloring);

    // Then, the coloring of the graphs in stdin starts
    start = clock();

    ssize_t read = 0;
    size_t len = 0;
    char *line = NULL;

    graph* g = NULL;
    // As long as there is something to read from stdin, we read it.
    while ((read = getline(&line, &len, stdin)) != -1) {
        // First we remove the \n from the end of the line:
        line[strcspn(line, "\n")] = 0;
        g = performComputation(g, line);
    }

    // The graphs have been processed, output:
    // Show the overview
    if (overview) {
        printOverview();
    }
    // We show that we have finished
    if (raw == 0) {
        printf("All graphs have been processed.\n");
    }

    // We don't forget to free line
    free(line);

    // Only if any graphs were input do we actually free the graph
    // When no graphs are found in stdin, the graph doesn't get created
    if (g != NULL) {
        freeGraph(g);
    }

    return 0;
}

int parseInt(char* str) {
    char *endptr;
    // fprintf(stderr,"%s\n", str);
    int value = strtol(str, &endptr, 10);
    if (endptr == str) {
        fprintf(stderr, "No digits were found.\n");
        exit(1);
    } else if (*endptr != '\0') {
        fprintf(stderr, "Invalid character: %c\n", *endptr);
        exit(1);
    }
    return value;
}

int parseBoolean(char* str) {
    if (strcmp("true", str) == 0) {
        return 1;
    }
    return 0;
}

void parseArguments(int argc, char **argv) {
    if (argc < 8 || argc > 8) {
        fprintf(stderr, "Expected exactly 7 arguments.\n");
        exit(1);
    }
    coloring = getColoring(argv[1]);
    overview = parseBoolean(argv[2]);
    raw = parseInt(argv[3]);
    minChrom = parseInt(argv[4]);
    checkCondition = parseInt(argv[6]);
    doSubdivide = parseBoolean(argv[7]);
}


void printOverview() {
    end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    for (int chrom = 0; chrom < SIZE(cNumbers); chrom++) {
        if (cNumbers[chrom]) printf("  %d graphs : chrom=%d\n", cNumbers[chrom], chrom);

    }
    int sum = 0;
    for (int chrom = 0; chrom < SIZE(cNumbers); chrom++) {
        sum += cNumbers[chrom];
    }
    printf("  %d graphs altogether; cpu=%f sec\n", sum, time_spent);
    // Reset
    for (int i = 0; i < SIZE(cNumbers); i++) {
        cNumbers[i] = 0;
    }
    start = clock();
}


graph* performComputation(graph* g, char line[]) {
    g = createGraph(g, line); // creates or modifies the graph to work with the amount of vertices

    if (coloring == ODD) {
        colorCheck = &isCorrectlyColoredOdd;
        handler = &handleOdd;
    }
    else if (coloring == PROPER) {
        colorCheck = &isCorrectlyColoredProper;
        handler = &handleProper;
    }
    else if (isUMColoring) {
        colorCheck = &isCorrectlyColoredUM;
        handler = &handleUM;
    }
    else {
        colorCheck = &isCorrectlyColoredCF;
        handler = &handleCF;
    }

    if (doSubdivide) {
        subdivide(g, 1);
    }

    int c = searchChromaticNumber(g, max(minChrom - 1, 1));

    // fprintf(stderr, "%d\n", checkCondition);
    // We check if the graph should get printed
    if (c < minChrom || (checkCondition != 0 && !isConditionMet(g, c))) {
        return g;
    }

    if (overview) {
        cNumbers[c] += 1;
    } else {
        switch (raw) {
            // We break here as we don't want to also output the default option
            case 1:
                printf("%d\n", c);
                break;
            case 2:
                // This option is only useful when filtering
                printf("%s\n", line);
                break;
            case 3: {
                int colors[g->numberOfVertices];
                memset(colors, 0, sizeof(colors));
                // For showing graphs, prints the colors
                char *extraInfo = "";
                if (checkCondition != 0) {
                    getColoringAfterCheck(g, c, colors);
                    extraInfo = getExtraInfoText(g->gadget_finder);
                } else {
                    getColors(g, colors);
                }
                printf("%s ", line);
                printColors(g, colors);
                printf("%s\n", extraInfo);
                break;
            }
                // case 4:
                //     System.out.println(line + " " + graph.getColorings()
                //             .stream().map(Arrays::toString).toList());
                //     break;
            default:
                printf("%s: %d\n", line, c);
        }
    }
    return g;
}

void freeGraph(graph* graph) {
    if (graph->gadget_finder)
        free(graph->gadget_finder);
    if (graph->changed)
        free(graph->changed);
    free(graph);
}


void printColors(graph* g, int colors[]) {
    printf("[");
    for (int i = 0; i < g->numberOfVertices - 1; i++) {
        printf("%d, ", colors[i]);
    }
    printf("%d]", colors[g->numberOfVertices - 1]);
}






