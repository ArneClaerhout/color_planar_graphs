import java.util.ArrayList;

public class Graph {

    /**
     * The list comprised of the vertices in this graph.
     */
    private ArrayList<Vertex> vertices = new ArrayList<>();



    public Graph(String graph6) {
        System.out.println(getNumberOfVertices(graph6.toCharArray()));
    }

    /**
     * A method for adding a vertex to a graph.
     */
    public void addVertex(Vertex v) {
        for (Vertex vertex : vertices) {
            if (vertex.equals(v)) {
                return;
            }
        }
        // We add it to the end as the order doesn't matter
        vertices.add(v);
    }

    /**
     * A method for checking whether this graph is correctly colored when using a given coloring method.
     *
     * @param   coloring
     *          The coloring method used.
     * @param   proper
     *          This is true if the coloring should be proper.
     * @param   open
     *          This is true if open neighbourhoods are used.
     *          This is false if closed neighbourhoods are used.
     */
    public boolean isCorrectlyColored(Coloring coloring, boolean proper, boolean open) {
        for (Vertex v : vertices) {
            if (!v.isCorrectlyColored(coloring, proper, open)) {
                return false;
            }
        }
        return true;
    }

    /**
     * This method checks whether the graph is correctly colored according to a certain coloring scheme.
     * This method assumes the coloring is proper and that we use open neighbourhoods.
     */
    public boolean isCorrectlyColored(Coloring coloring) {
        return isCorrectlyColored(coloring, true, true);
    }


    private int getNumberOfVertices(char[] graphString) {
        int index = 0;
        if (graphString[index] == '>') { // Skip >>graph6<< header.
            index += 10;
        }

        if(graphString[index] < 126) { // 0 <= n <= 62
            return (int) graphString[index] - 63;
        }

        else if(graphString[++index] < 126) {
            int number = 0;
            for(int i = 2; i >= 0; i--) {
                number |= (graphString[index++] - 63) << i*6;
            }
            return number;
        }

        else if (graphString[++index] < 126) {
            int number = 0;
            for (int i = 5; i >= 0; i--) {
                number |= (graphString[index++] - 63) << i*6;
            }
            return number;
        }

        return 0;
    }



}
