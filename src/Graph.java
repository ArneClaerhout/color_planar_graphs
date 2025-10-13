import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

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
     */
    public boolean isCorrectlyColored(Coloring coloring) {
        for (Vertex v : vertices) {
            if (!v.isCorrectlyColored(coloring)) {
                return false;
            }
        }
        return true;
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

    public int findChromaticNumberNaive(Coloring coloring) {
        int n = coloring.getMaxChromaticNumber();

        for (int i = 1; i <= n; i++) {
            if (naiveAlgorithm(coloring, i, 0)) {
                return i;
            }
        }
        return 0;
    }

    private boolean naiveAlgorithm(Coloring coloring, int maxColor, int index) {
        if (index >= vertices.size()) {
            // We reached the end of the list
            return isCorrectlyColored(coloring);
        }

        for (int i = 1; i <= maxColor; i++) {
            vertices.get(index).changeColor(i);
            if (naiveAlgorithm(coloring, maxColor, index+1)) {
                return true;
            }
        }

        return false;

    }

}
