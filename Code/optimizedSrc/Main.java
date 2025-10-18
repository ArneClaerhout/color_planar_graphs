import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Arrays;

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

        // Last: whether the output should be raw
        boolean raw = false;
        if (args[1].equals("true")) {
            // We know there is only an extra argument if the output should be raw
            raw = true;
        }


        if (!raw) System.out.println("Received coloring: " + coloring);


        // We start reading the graphs
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

        String line;
        // As long as there is something to read from stdin, we read it.
        while ((line = reader.readLine()) != null) {

            Graph graph = new Graph(line);
            if (!raw) System.out.print(line + ": ");
            System.out.println(graph.findChromaticNumberNaive(coloring));

        }
        if (!raw) System.out.println("All graphs have been processed.");


    }

}
