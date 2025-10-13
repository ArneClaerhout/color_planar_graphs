import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;

public class Main {

    /***********************************
     ********** INPUT READING **********
     **********************************/

    public static void main(String[] args) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
        String line;


        String inputColoring = args[0];
        System.out.println("Received coloring: " + inputColoring);

        Coloring coloring = Coloring.getColoring(inputColoring);

        // As long as there is something to read from stdin, we read it.
        while ((line = reader.readLine()) != null) {

            Graph graph = new Graph(line);

            System.out.println(graph.findChromaticNumberNaive(coloring));


        }

        System.out.println("No more input");
    }

}
