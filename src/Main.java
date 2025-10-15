import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {

    /***********************************
     ********** INPUT READING **********
     **********************************/

    public static void main(String[] args) throws IOException {
        ArgumentParser argsParser = new ArgumentParser(args);
        String line;

        String inputColoring = argsParser.getValue("-c");

        Coloring coloring = Coloring.getColoring(inputColoring);
        System.out.println("Received coloring: " + coloring);


        if (argsParser.getValue("-m") == null) {
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

            // As long as there is something to read from stdin, we read it.
            while ((line = reader.readLine()) != null) {

                Graph graph = new Graph(line);
                System.out.print(line + ": ");
                System.out.println(graph.findChromaticNumberNaive(coloring));

            }
            System.out.println("All graphs have been processed.");


        } else {
            // Manual is requested
            line = argsParser.getValue("-m");

            Graph graph = new Graph(line);
            System.out.print(line + ": ");
            System.out.println(graph.findChromaticNumberNaive(coloring));

        }



    }

}
