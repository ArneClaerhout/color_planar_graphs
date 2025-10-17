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


        System.out.println("Received coloring: " + coloring);


        // We start reading the graphs
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

        String line;
        // As long as there is something to read from stdin, we read it.
        while ((line = reader.readLine()) != null) {

            Graph graph = new Graph(line);
            System.out.print(line + ": ");
            System.out.println(graph.findChromaticNumberNaive(coloring));

        }
        System.out.println("All graphs have been processed.");


    }

}
