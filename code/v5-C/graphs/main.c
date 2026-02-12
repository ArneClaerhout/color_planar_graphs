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

uint64_t start;
uint64_t end;

int main(int argc, char **argv) {

    parseArguments(argc, argv);

    open = coloringIsOpen(coloring);
    proper = coloringIsProper(coloring);
    um = coloringIsUM(coloring);

    uint64_t start = clock();

    ssize_t read;
    size_t len = 0;
    char* line;

    // As long as there is something to read from stdin, we read it.
    while ((read = getline(&line, &len, stdin)) != -1) {

        // First we remove the \n from the end of the line:
        line[strcspn(line, "\n")] = 0;

        graph* graph = createGraph(line);

        const int c = findChromaticNumberOptimized(graph, coloring, max(minChrom - 1, 1), (raw == 4));

        // We check if the graph should get printed
        if (c < minChrom || (checkCondition != 0 && isConditionMet(graph->counter, c))) {
            continue;
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
                    int colors[graph->numberOfVertices];
                    // For showing graphs, prints the colors
                    if (checkCondition != 0) {
                        getColoringAfterCheck(graph->counter, c, colors);
                    } else {
                        getColors(graph, colors);
                    }
                    printf("%s [", line);
                    for (int i = 0; i < graph->numberOfVertices; i++) {
                        printf("%d ", colors[i]);
                    }
                    printf("]\n");
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

        for (int i = 0; i < graph->numberOfVertices; i++) {
            free(graph->verticesIndexed[i]);
        }
        free(graph->counter);

        free(graph);
    }

    if (overview) {
        printOverview();
    }

    if (raw == 0) {
        printf("All graphs have been processed.\n");
    }

    // We don't forget to free line
    free(line);

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






