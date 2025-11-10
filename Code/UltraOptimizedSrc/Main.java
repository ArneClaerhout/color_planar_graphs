import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.TreeMap;

public class Main {

    /***********************************
     ********** INPUT READING **********
     **********************************/

    public static void main(String[] args) throws IOException {

        boolean debugging = false;
        debugging = true;

        if (!debugging) {

            // We get the arguments from our bash script, we know the order:
            // First: the coloring
            Coloring coloring = Coloring.getColoring(args[0]);

            // Second: overview
            boolean overview = false;
            TreeMap<Integer, Integer> cNumbers = new TreeMap<>();
            if (args[1].equals("true")) {
                // We know there is only an extra argument if the output should be raw
                overview = true;
            }

            // Third: whether the output should be raw
            int raw = Integer.parseInt(args[2]);

            // Lastly: adding a filter to the output (min. chromatic number)
            int minChrom = 0;
            try {
                minChrom = Integer.parseInt(args[3]);
            } catch(NumberFormatException e) {
                // Filter isn't a number: we set it to 0
                minChrom = 0;
            }


            if (raw == 0) System.out.println("Received coloring: " + coloring);


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
                                System.out.println("All graphs have been processed for this filter.");
                            }
                            // Restart the start
                            start = System.currentTimeMillis();
                            continue;
                    }
                }

                int[][] adjM = Graph.getAdjacencyMatrix(line);

                Graph graph = new Graph(adjM);
//                int count = 0;
//                for (int v = 0; v < adjM.length; v++) {
//                    count = count + graph.vertices[v].getDegree();
//                }
                int c;
//                if (Math.pow(adjM.length, 2)/count > 4) {
//                    GraphPQ graphPQ = new GraphPQ(adjM);
//                    c = graphPQ.findChromaticNumberOptimized(coloring);
//                } else {
                    c = graph.findChromaticNumberOptimized(coloring);
//                }



                if (c >= minChrom) {
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
                                System.out.println(line + " " + Arrays.toString(graph.getColors()));
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
                System.out.println("All graphs have been processed.");
            }

        } else {
            // This is the debugging code section

            ArrayList<String> graphs = new ArrayList<>(){{
//                add("L|eKKF`WJ?k@Nw");
//                add("L|eKKEDoJxk@@w");
//                add("L|eKKF`WI?kBNw");
//                add("L~eKKF@oI@j{?M");
//                add("I|~KMLKBG");
//                add("E|tw");
//                add("G|mnMC");
                add("I~fIIDBVg");
            }};

            for (String line : graphs) {
                Graph graph = new Graph(line);


                int c = graph.findChromaticNumberOptimized(Coloring.getColoring("odd"));
                System.out.println(Arrays.toString(graph.getColors()));
                System.out.println(c);
            }

//            Graph graphnew = new Graph("O~eK]f@SOuA@EBE?_X?AP");
//            System.out.println(Arrays.deepToString(Graph.getAdjacencyMatrix("O~eK]f@SOuA@EBE?_X?AP")));
//            graphnew.colorGraph(new int[]{1, 2, 3, 2, 2, 3, 4, 3, 5, 1, 5, 4, 2, 1, 1, 3});
//            graphnew.isCorrectlyColored(Coloring.pUMc);







        }



    }

}
