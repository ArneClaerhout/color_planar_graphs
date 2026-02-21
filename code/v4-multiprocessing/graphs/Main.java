package graphs;

import java.io.*;
import java.util.*;

public class Main {

    /***********************************
     ********** INPUT READING **********
     **********************************/

    static Coloring coloring;
    static boolean open;
    static boolean proper;
    static boolean um;
    static boolean overview;
    static TreeMap<Integer, Integer> cNumbers;
    static int minChrom = 0;
    static int raw;
    static int method;
    static int checkCondition;

    static long start;
    static long end;

    public static void main(String[] args) throws IOException {

        boolean debugging = false;
//         debugging = true;

        if (!debugging) {

            // We get the arguments from our bash script, we know the order:
            parseArguments(args);

            if (raw == 0)
                System.err.println("Received coloring: " + coloring);
            if (raw == 0 && method != 0)
                System.err.println("Received method: "
                        + ((method == 1) ? "Priority Queues" : (method == 2) ? "Linked Lists" : "Bitsets"));
            // This is still done to stderr so that it doesn't interfere with other things

            // We start reading the graphs
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

            String line;

            start = System.currentTimeMillis();

            // As long as there is something to read from stdin, we read it.
            while ((line = reader.readLine()) != null) {

                // Check if the line is an option to change
                if (line.startsWith(":")) {
                    changeOptions(line.split(" "));
                    continue;
                }

                int c;
                Graph graph;
                switch (method) {
                    case 1:
                        graph = new GraphPQ(line);
                        break;
                    case 2:
                        graph = new GraphLL(line);
                        break;
                    case 3:
                        graph = new GraphBS(line);
                        break;
                    default:
                        graph = new Graph(line);
                        break;
                }
                c = graph.findChromaticNumberOptimized(coloring, Integer.max(minChrom - 1, 1), open, proper, um, checkCondition, (raw == 4));

                // We check if the graph should get printed
                if (c < minChrom || (checkCondition != 0
                        && !graph.counter.isConditionMet())) {
                    continue;
                }

                boolean skip = false;
                if (checkCondition != 0 && coloring == Coloring.iUMc) {
                    skip = true;
                    for (int i = 0; i < graph.counter.getCondition().length - 1; i++) {
                        if (graph.counter.getCondition(i) == 0 || graph.counter.getCondition(i) == graph.maxColoring)
                            continue;
                        if (checkSpecialCaseIUMC(Graph.getAdjacencyMatrix(line),
                                (graph.maxColoring & ~graph.counter.getCondition(i)), i)) {
                            skip = false;
                            break;
                        }
                    }
                }
                if (skip) continue;

                if (overview) {
                    cNumbers.merge(c, 1, Integer::sum);
                } else {
                    switch (raw) {
                        // We break here as we don't want to also output the default option
                        case 1:
                            System.out.println(c);
                            break;
                        case 2:
                            // This option is only useful when filtering
                            System.out.println(line);
                            break;
                        case 3:
                            // For showing graphs, prints the colors
                            if (checkCondition != 0) {
                                int[] colors = graph.counter.getColoringAfterCheck();
                                System.out.println(line + " " + Arrays.toString(colors));
                                // }
                            } else {
                                int[] colors = graph.getColors();
                                System.out.println(line + " " + Arrays.toString(colors));
                            }
                            break;
                        case 4:
                            System.out.println(line + " " + graph.getColorings()
                                    .stream().map(Arrays::toString).toList());
                            break;
                        default:
                            System.out.println(line + ": " + c);
                    }
                }

            }

            if (overview) {
                printOverview();
            }

            if (raw == 0) {
                System.err.println("All graphs have been processed.");
            }

        } else {
            // This is the debugging code section

             File file = new
             File("/home/arne/Bachelorproef/code/v5-C/outputs/2026-02-16-18-49-09.txt");
             BufferedReader br = new BufferedReader(new FileReader(file));
             String line;
             while((line = br.readLine()) != null){
                 Graph graph = new Graph(line);
                 int c = graph.findChromaticNumberOptimized(Coloring.iUMo, Integer.max(0 - 1, 1), true, false, true, 0, false);
                 if (c == 0) {
                     System.out.println(line);
                     break;
                 }
             }

//             ArrayList<String> graphs = new ArrayList<>(){{
//            // add("a???????E?S?c?a?O_CC?_OA?_B??A_?@@??OG?A?_?E???E???D???AO???a???CC???K????K????E????@_????S????");
//            // add("L|eKKEDoJxk@@w");
//            // add("L|eKKF`WI?kBNw");
//            // add("L~eKKF@oI@j{?M");
//            // add("I|~KMLKBG");
//             add("G@Ezu[");
//            // add("G|mnMC");
////             add("I~fIIDBVg");
//             }};
//             //
//             for (String line2 : graphs) {
//             Graph graph = new Graph(line2);
//             //
//             int c = graph.findChromaticNumberOptimized(coloring, Integer.max(minChrom - 1, 1), open, proper, um, checkCondition, (raw == 4));

//             if (c < minChrom || (checkCondition != 0
//                     && Arrays.stream(graph.counter.getCondition())
//                     .allMatch(n -> (n == graph.maxColoring || n == 0)))) {
//                 continue;
//             }
//
//             for (int i = 0; i < 10; i++) {
//                 if (graph.counter.getCondition(i) == 0 || graph.counter.getCondition(i) == graph.maxColoring)
//                     continue;
//                 if (checkSpecialCaseIUMC(Graph.getAdjacencyMatrix(line2),
//                         (graph.maxColoring & ~graph.counter.getCondition(i)), i)) {
//                     System.out.println("yes");
//                     break;
//                 }
//             }
//             System.out.println(c);
//             }

            // Graphs.Graph graphnew = new Graphs.Graph("O~eK]f@SOuA@EBE?_X?AP");
            // System.out.println(Arrays.deepToString(Graphs.Graph.getAdjacencyMatrix("O~eK]f@SOuA@EBE?_X?AP")));
            // graphnew.colorGraph(new int[]{1, 2, 3, 2, 2, 3, 4, 3, 5, 1, 5, 4, 2, 1, 1,
            // 3});
            // graphnew.isCorrectlyColored(Graphs.Coloring.pUMc);

        }

        System.exit(0);

    }

    public static void parseArguments(String[] args) {
        // First: the coloring
        coloring = Coloring.getColoring(args[0]);

        open = Coloring.isOpen(coloring);
        proper = Coloring.isProper(coloring);
        um = Coloring.isUniqueMaximum(coloring);

        // Second: overview
        overview = false;
        cNumbers = new TreeMap<>();
        if (args[1].equals("true")) {
            // We know there is only an extra argument if the output should be raw
            overview = true;
        }

        // Third: whether the output should be raw
        raw = Integer.parseInt(args[2]);

        // Fourth: adding a filter to the output (min. chromatic number)
        try {
            minChrom = Integer.parseInt(args[3]);
        } catch (NumberFormatException e) {
            // Filter isn't a number: we set it to 0
            minChrom = 0;
        }

        // Fifth: choosing the method
        method = Integer.parseInt(args[4]);

        // Lastly: whether we check a condition
        checkCondition = Integer.parseInt(args[5]);
    }

    public static void changeOptions(String[] parts) {

        switch (parts[0]) {
            case ":coloring":
                coloring = Coloring.getColoring(parts[1]);
                open = Coloring.isOpen(coloring);
                proper = Coloring.isProper(coloring);
                um = Coloring.isUniqueMaximum(coloring);
            case ":minChrom":
                minChrom = Integer.parseInt(parts[1]);
            case ":raw":
                raw = Integer.parseInt(parts[1]);
            case ":overview":
                overview = Boolean.parseBoolean(parts[1]);
            case ":print":
                System.out.println(parts[1] + ":");
            case ":update":
                // We are changing filter, output the overview
                if (overview) {
                    printOverview();
                }
                if (raw == 0) {
                    System.err.println("All graphs have been processed for this filter.");
                }
        }
    }

    public static void printOverview() {
        end = System.currentTimeMillis();
        String finalTime = String.format("%.2f", (end - start) / 1000.0); // in seconds
        for (Integer chrom : cNumbers.keySet()) {
            System.out.println("  " + cNumbers.get(chrom) + " graphs : chrom=" + chrom);
            // This is in sorted order as it was stored in a TreeMap
        }
        System.out.println("  " + cNumbers.values().stream().mapToInt(x -> x).sum()
                + " graphs altogether; cpu=" + finalTime + " sec");
        // Reset
        cNumbers = new TreeMap<>();
        start = System.currentTimeMillis();
    }

    private static boolean checkSpecialCaseIUMC(int[][] adjMatrix, long indicesInOwnGraph, int indexColor) {
        for (long k = indicesInOwnGraph; k != 0; k &= k - 1) {
            int index = Long.numberOfTrailingZeros(k);

            // We start from a Graphs.Graph made from a very simple graph needing 3 colors
            Graph graphStart = new Graph("E|tw");
            int n = graphStart.numberOfVertices;
            for (int i = 0; i < n; i++) {
                graphStart.addGraphToIndex(adjMatrix, i, index);
            }
            int c = graphStart.findChromaticNumberOptimized(Coloring.iUMc, 2, false, false, true, 1, false);
            System.err.println(c);
            if ((checkCondition == 0 || graphStart.counter.getCondition(indexColor) != graphStart.maxColoring)) {
                // The condition is found
                System.err.println(c);
                // System.err.println(graphStart.numberOfVertices);
                return true;
            }
        }
        return false;

    }

}
