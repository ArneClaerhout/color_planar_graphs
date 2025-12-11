import java.io.*;
import java.util.*;

public class Main {

    /***********************************
     ********** INPUT READING **********
     **********************************/

    static int minChrom = 0;

    public static void main(String[] args) throws IOException {

        boolean debugging = false;
//        debugging = true;

        if (!debugging) {

            // We get the arguments from our bash script, we know the order:
            // First: the coloring
            Coloring coloring = Coloring.getColoring(args[0]);

            boolean open = Coloring.isOpen(coloring);
            boolean proper = Coloring.isProper(coloring);
            boolean um = Coloring.isUniqueMaximum(coloring);

            // Second: overview
            boolean overview = false;
            TreeMap<Integer, Integer> cNumbers = new TreeMap<>();
            if (args[1].equals("true")) {
                // We know there is only an extra argument if the output should be raw
                overview = true;
            }

            // Third: whether the output should be raw
            int raw = Integer.parseInt(args[2]);

            // Fourth: adding a filter to the output (min. chromatic number)
            try {
                minChrom = Integer.parseInt(args[3]);
            } catch(NumberFormatException e) {
                // Filter isn't a number: we set it to 0
                minChrom = 0;
            }

            // Fifth: choosing the method
            int method = Integer.parseInt(args[4]);

            // Lastly: whether we check a condition
            boolean checkCondition = Boolean.parseBoolean(args[5]);


            if (raw == 0) System.err.println("Received coloring: " + coloring);
            if (raw == 0 && method != 0) System.err.println("Received method: " + ((method == 1) ? "Priority Queues" : (method == 2) ? "Linked Lists" : "Bitsets"));
            // This is still done to stderr so that it doesn't interfere with other things


            // We start reading the graphs
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

            String line;

            long start = System.currentTimeMillis();

            // As long as there is something to read from stdin, we read it.
            while ((line = reader.readLine()) != null) {

                // Check if the line is an option to change
                if (line.startsWith(":")) {
                    String[] parts = line.split(" ");

                    switch (parts[0]) {
                        case ":coloring":
                            coloring = Coloring.getColoring(parts[1]);
                            open = Coloring.isOpen(coloring);
                            proper = Coloring.isProper(coloring);
                            um = Coloring.isUniqueMaximum(coloring);
                            continue;
                        case ":minChrom":
                            minChrom = Integer.parseInt(parts[1]);
                            continue;
                        case ":raw":
                            raw = Integer.parseInt(parts[1]);
                            continue;
                        case ":overview":
                            overview = Boolean.parseBoolean(parts[1]);
                            continue;
                        case ":print":
                            System.out.println(parts[1] + ":");
                            continue;
                        case ":update":
                            // We are changing filter, output the overview
                            long end = System.currentTimeMillis();
                            String finalTime = String.format("%.2f", (end - start)/1000.0); // in seconds
                            if (overview) {
                                for (Integer chrom : cNumbers.keySet()) {
                                    System.out.println("  " + cNumbers.get(chrom) + " graphs : chrom=" + chrom);
                                    // This is in sorted order as it was stored in a TreeMap
                                }
                                System.out.println("  " + cNumbers.values().stream().mapToInt(x -> x).sum() + " graphs altogether; cpu=" + finalTime + " sec");
                                // Reset
                                cNumbers = new TreeMap<>();
                            }
                            if (raw == 0) {
                                System.err.println("All graphs have been processed for this filter.");
                            }
                            // Restart the start
                            start = System.currentTimeMillis();
                            continue;
                    }
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
                c = graph.findChromaticNumberOptimized(coloring, open, proper, um, checkCondition, (raw == 4));

                if (c >= minChrom && (!checkCondition || graph.counter.getCondition() != graph.maxColoring)) {
                    if (overview) {
                        cNumbers.merge(c, 1, Integer::sum);
                    } else {
                        switch (raw) {
                            // We break here as we don't want to also output the default option
                            case 1:
                                System.out.println(c); break;
                            case 2:
                                System.out.println(line); break;
                                // This option is only useful when filtering
                            case 3:
                                if (checkCondition) {
                                    int[] colors = new int[graph.numberOfVertices];
                                    int count = 0;
                                    for (long k = (graph.maxColoring & ~graph.counter.getCondition()); k != 0; k &= k - 1) {
                                        int index = Long.numberOfTrailingZeros(k);
                                        colors[index] = graph.chromaticNumber;
                                        count++;
                                    }
//                                    if (count > 1) {
                                        System.out.println(line + " " + Arrays.toString(colors));
//                                    }
                                } else {
                                    int[] colors = graph.getColors();
                                    System.out.println(line + " " + Arrays.toString(colors));
                                }
                                break;
                            case 4:
                                System.out.println(line + " " + graph.getColorings()
                                        .stream().map(Arrays::toString).toList());
                                break;
                            default: System.out.println(line + ": " + c);
                        }
                    }
                }



            }

            long end = System.currentTimeMillis();

            String finalTime = String.format("%.2f", (end - start)/1000.0); // in seconds

            if (overview) {
                for (Integer chrom : cNumbers.keySet()) {
                    System.out.println("  " + cNumbers.get(chrom) + " graphs : chrom=" + chrom);
                    // This is in sorted order as it was stored in a TreeMap
                }
                System.out.println("  " + cNumbers.values().stream().mapToInt(x -> x).sum() + " graphs altogether; cpu=" + finalTime + " sec");
            }

            if (raw == 0) {
                System.err.println("All graphs have been processed.");
            }

        } else {
            // This is the debugging code section

//            File file = new File("/home/arne/Bachelorproef/Code/veryOptimizedSrc/outputs/2025-11-20-17-40-35.txt");
//            BufferedReader br = new BufferedReader(new FileReader(file));
//            String line;
//            while((line = br.readLine()) != null){
//                Graph graph = new Graph(line);
//                int c = graph.findChromaticNumberOptimized(Coloring.getColoring("proper"), true, true, false, false, false);
//                if (c == 0) {
//                    System.out.println(line);
//                    break;
//                }
//            }

//            ArrayList<String> graphs = new ArrayList<>(){{
//                add("a???????E?S?c?a?O_CC?_OA?_B??A_?@@??OG?A?_?E???E???D???AO???a???CC???K????K????E????@_????S????");
//                add("L|eKKEDoJxk@@w");
//                add("L|eKKF`WI?kBNw");
//                add("L~eKKF@oI@j{?M");
//                add("I|~KMLKBG");
//                add("E|tw");
//                add("G|mnMC");
//                add("I~fIIDBVg");
//            }};
//
//            for (String line2 : graphs) {
//                Graph graph = new Graph(line2);
////
//                int c = graph.findChromaticNumberOptimized(Coloring.getColoring("odd"), true, true, false, false, false);
////                System.out.println(Arrays.toString(graph.getColors()));
//                System.out.println(c);
//            }

//            Graph graphnew = new Graph("O~eK]f@SOuA@EBE?_X?AP");
//            System.out.println(Arrays.deepToString(Graph.getAdjacencyMatrix("O~eK]f@SOuA@EBE?_X?AP")));
//            graphnew.colorGraph(new int[]{1, 2, 3, 2, 2, 3, 4, 3, 5, 1, 5, 4, 2, 1, 1, 3});
//            graphnew.isCorrectlyColored(Coloring.pUMc);







        }

        System.exit(0);

    }

}
