#include "colorings.h"
#include "vertex.h"
#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "counter.h"


enum colorings coloring;
int overview;
int cNumbers[10];
int minChrom;
int raw;
int checkCondition;
int open;
int proper;
int um;

graph* g;
int lengthOfGraph;

int (*handler)(int, vertex*, int, uint64_t, int);
int (*colorCheck)(vertex*, vertex*, int);


uint64_t start;
uint64_t end;

int main(int argc, char **argv) {

    int debugging = 0;

    if (!debugging) {
        parseArguments(argc, argv);

        open = coloringIsOpen(coloring);
        proper = coloringIsProper(coloring);
        um = coloringIsUM(coloring);

        uint64_t start = clock();

        ssize_t read;
        size_t len = 0;
        char* line;

        int previousN = 0;
        // As long as there is something to read from stdin, we read it.
        while ((read = getline(&line, &len, stdin)) != -1) {
            // First we remove the \n from the end of the line:
            line[strcspn(line, "\n")] = 0;
            previousN = performComputation(previousN, line);
        }

        if (overview) {
            printOverview();
        }

        if (raw == 0) {
            printf("All graphs have been processed.\n");
        }

        // We don't forget to free line
        free(line);
        freeGraph(g);
    } else {
        overview = 1;
        minChrom = 0;
        raw = 0;
        checkCondition = 1;
        coloring = pUMo;
        open = coloringIsOpen(coloring);
        proper = coloringIsProper(coloring);
        um = coloringIsUM(coloring);

        uint64_t start = clock();

        ssize_t read;
        size_t len = 0;
        char* line;

        // FILE *fptr = fopen("../outputs/2026-02-16-18-49-09.txt", "r");

        int previousN = 0;
        // As long as there is something to read from stdin, we read it.
        // while ((read = getline(&line, &len, fptr)) != -1) {
            // First we remove the \n from the end of the line:
            // line[strcspn(line, "\n")] = 0;
        line = "C~";
            previousN = performComputation(previousN, line);
        // }

        if (overview) {
            printOverview();
        }

        if (raw == 0) {
            printf("All graphs have been processed.\n");
        }

        // We don't forget to free line
        // free(line);
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
    if (argc > 7) {
        fprintf(stderr, "Too many arguments.\n");
        exit(1);
    }
    coloring = getColoring(argv[1]);
    overview = parseBoolean(argv[2]);
    raw = parseInt(argv[3]);
    minChrom = parseInt(argv[4]);
    checkCondition = parseInt(argv[6]);
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


int performComputation(int previousN, char line[]) {
    g = createGraph(previousN, line); // creates or modifies the graph to work with the amount of vertices

    if (coloring == ODD) {
        colorCheck = &isCorrectlyColoredOdd;
        handler = &handleOdd;
    }
    else if (coloring == PROPER) {
        colorCheck = &isCorrectlyColoredProper;
        handler = &handleProper;
    }
    else if (um) {
        colorCheck = &isCorrectlyColoredUM;
        handler = &handleUM;
    }
    else {
        colorCheck = &isCorrectlyColoredCF;
        handler = &handleCF;
    }

    const int c = findChromaticNumberOptimized(max(minChrom - 1, 1), (raw == 4));

    // fprintf(stderr, "%d\n", c);
    // We check if the graph should get printed
    if (c < minChrom || (checkCondition != 0 && !isConditionMet(g->counter, c))) {
        return g->numberOfVertices;
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
                    getColoringAfterCheck(g->counter, c, colors);
                    extraInfo = getExtraInfoText(g->counter);
                } else {
                    getColors(colors);
                }
                printf("%s [", line);
                for (int i = 0; i < g->numberOfVertices - 1; i++) {
                    printf("%d, ", colors[i]);
                }
                printf("%d]%s\n", colors[g->numberOfVertices - 1], extraInfo);
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
    return g->numberOfVertices;
}

void freeGraph(graph* graph) {
    free(graph->counter);
    free(graph->changed);
    free(graph);

}






