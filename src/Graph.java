import java.util.ArrayList;

public class Graph {

    /**
     * The list comprised of the vertices in this graph.
     */
    private ArrayList<Vertex> vertices = new ArrayList<>();



    public Graph(String graph6) {
        char[] graphArray = graph6.toCharArray();
        int n = getNumberOfVertices(graphArray);
        int[][] adjMatrix = getAdjacencyMatrix(graphArray, n);

        this.vertices = findVerticesList(adjMatrix);

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


    /**
     * This method finds the number of vertices for a given graph6 formatted list of characters (= String).
     * This is a simplified version of this method that only works for graphs of up to 62 vertices.
     */
    private int getNumberOfVertices(char[] graphString) {
        int index = 0;
        if (graphString[index] == '>') {
            index += 10;
        }

        // Only works when #vertices < 63, we won't go higher than that
        if(graphString[index] < 126) {
            return (int) graphString[index] - 63;
        }

        return 0;
    }



    private int[][] getAdjacencyMatrix(char[] graphString, int n) {
        int[][] adjMatrix = new int[n][n];
        int index = 1; // First index as index 0 is the vertex count

        int bitPos = 0; // Bit position per index
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                // Go through the entire adjacency matrix
                int part6bits = graphString[index] - 63;

                int bitShift = 5 - (bitPos % 6);
                // The amount you have to bitshift the number to check bitPos here:
                boolean edge = ((part6bits >> bitShift) & 1) == 1;
                if (edge) {
                    adjMatrix[i][j] = 1;
                    adjMatrix[j][i] = 1;
                    // The matrix is symmetrical
                }

                bitPos++;
                if (bitPos % 6 == 0) index++;
            }
        }
        return adjMatrix;
    }


    private ArrayList<Vertex> findVerticesList(int[][] adjMatrix) {
        ArrayList<Vertex> vertices = new ArrayList<>();

        for (int i = 0; i < adjMatrix.length; i++) {
            vertices.add(new Vertex());
        }

        for (int i = 0; i < adjMatrix.length; i++) {
            for (int j = i+1; j < adjMatrix[i].length; j++) {
                if(adjMatrix[i][j] == 1) {
                    vertices.get(i).addNeighbour(vertices.get(j));
                }
            }
        }

        return vertices;
    }

}
