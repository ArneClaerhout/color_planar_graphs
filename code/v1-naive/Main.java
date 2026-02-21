import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.sql.SQLOutput;
import java.util.Arrays;
import java.util.HashMap;
import java.util.TreeMap;

public class Main {

    /***********************************
     ********** INPUT READING **********
     **********************************/

    public static void main(String[] args) throws IOException {

        // We get the arguments from our bash script, we know the order:
        // First: the coloring
        Coloring coloring = Coloring.getColoring(args[0]);
        // Second: filter
        //TODO

        // Third: overview
        boolean overview = false;
        TreeMap<Integer, Integer> cNumbers = new TreeMap<>();
        if (args[1].equals("true")) {
            // We know there is only an extra argument if the output should be raw
            overview = true;
        }

        // Last: whether the output should be raw
        boolean raw = false;
        if (args[2].equals("true")) {
            // We know there is only an extra argument if the output should be raw
            raw = true;
        }


        if (!raw) System.err.println("Received coloring: " + coloring);


        // We start reading the graphs
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

        String line;

        long start = System.currentTimeMillis();

        // As long as there is something to read from stdin, we read it.
        while ((line = reader.readLine()) != null) {

            Graph graph = new Graph(line);

            if (overview) {
                int c = graph.findChromaticNumberNaive(coloring);
                cNumbers.merge(c, 1, Integer::sum);
            } else {
                if (!raw) System.out.print(line + ": ");
                System.out.println(graph.findChromaticNumberNaive(coloring));
            }


        }

        long end = System.currentTimeMillis();

        String finalTime = String.format("%.2f", (end - start)/1000.0); // in seconds

        if (!raw) {
            System.err.println("All graphs have been processed.");
            System.err.println("Final time: " + finalTime);
        }

        if (overview) {
            for (Integer c : cNumbers.keySet()) {
                System.out.println(cNumbers.get(c) + " graphs : chrom=" + c);
                // This is in sorted order as it was stored in a TreeMap
            }
            System.out.println(cNumbers.values().stream().mapToInt(x -> x).sum() + " graphs altogether; cpu=" + finalTime + " sec");
        }


    }

}
