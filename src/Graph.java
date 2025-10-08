import java.util.ArrayList;

public class Graph {

    /**
     * The list comprised of the vertices in this graph.
     */
    private ArrayList<Vertex> vertices = new ArrayList<>();



    public Graph() {

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



}
