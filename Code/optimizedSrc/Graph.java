import java.util.Arrays;
import java.util.Comparator;

public class Graph {

    /**
     * The array comprised of the vertices in this graph.
     */
    private Vertex[] vertices;

    public Graph(String graph6) {
        char[] graphArray = graph6.toCharArray();
        int n = getNumberOfVertices(graphArray);
        int[][] adjMatrix = getAdjacencyMatrix(graphArray, n);

        this.vertices = findVerticesList(adjMatrix, n);
    }

//    /**
//     * A method for adding a vertex to a graph.
//     */
//    public void addVertex(Vertex v) {
//        for (Vertex vertex : vertices) {
//            if (vertex.equals(v)) {
//                return;
//            }
//        }
//        // We add it to the end as the order doesn't matter
//        vertices.add(v);
//    }

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

        // Only works when #vertices < 63, we won't go higher than that
        if(graphString[index] < 126) {
            return (int) graphString[index] - 63;
        }

        return 0;
    }


    /**
     * A method for finding the adjacency matrix given a string in the graph6 format.
     *
     * @param   graphString
     *          The string in graph6 format which should be converted.
     * @param   n
     *          The amount of vertices for this graph.
     */
    private int[][] getAdjacencyMatrix(char[] graphString, int n) {
        int[][] adjMatrix = new int[n][n];
        int index = 1; // First index as index 0 is the vertex count

        int bitPos = 0; // Bit position per index
        for (int i = 1; i < n; i++) {
            for (int j = 0; j < i; j++) {
                // Go through the entire adjacency matrix
                int part6bits = graphString[index] - 63;

                int bitShift = 5 - bitPos;
                // The amount you have to bitshift the number to check bitPos here:
                boolean edge = ((part6bits >> bitShift) & 1) == 1;
                if (edge) {
                    adjMatrix[i][j] = 1;
                    adjMatrix[j][i] = 1;
                    // The matrix is symmetrical
                }

                bitPos++;
                if (bitPos % 6 == 0) {
                    index++;
                    bitPos = 0;
                }
            }
        }
        return adjMatrix;

    }

    /**
     * A method for creating a vertices array from a given adjacency matrix.
     *
     * @param   adjMatrix
     *          The adjacency matrix to find the vertices for.
     * @param   n
     *          The amount of vertices.
     * @return  The array of vertices with each vertex instantiated.
     */
    private Vertex[] findVerticesList(int[][] adjMatrix, int n) {
        Vertex[] vertices = new Vertex[n];

        for (int i = 0; i < n; i++) {
            vertices[i] = new Vertex();
        }

        for (int i = 0; i < adjMatrix.length; i++) {
            for (int j = i+1; j < adjMatrix[i].length; j++) {
                if(adjMatrix[i][j] == 1) {
                    vertices[i].addNeighbour(vertices[j]);
                }
            }
        }

        return vertices;
    }


    /**
     * A method for finding the proper, odd, conflict-free
     * or unique-maximum chromatic number of a graph.
     * This is done using a memorization of the available colors
     * if the chosen coloring method is proper.
     *
     * @param   coloring
     *          The chosen coloring method of which this method is finding the chromatic number of.
     */
    public int findChromaticNumberOptimized(Coloring coloring) {
        int n = coloring.getMaxChromaticNumber();
        boolean proper = Coloring.isProper(coloring);

        for (int i = 1; i <= n; i++) {
            for (Vertex v : vertices) {
                v.setMaxAvailableColors(i);
            }
            if (optimizedAlgorithm(coloring, proper, i, 0)) {
                return i;
            }
        }
        return 0;
    }

    /**
     * A method for running the optimized algorithm using the memorization of available colors for each vertex.
     *
     * @param   coloring
     *          The coloring of which the algorithm should to try to find a solution for.
     * @param   proper
     *          Whether the coloring is proper.
     * @param   maxColor
     *          The maximum color possible for this coloring method.
     * @param   index
     *          The index of where this algorithm is working.
     * @return  True if the algorithm found a coloring for this graph.
     *          The colors of each of the vertex objects in vertices are the correct colors.
     *          False if there is no possible coloring for this maxColor.
     */
    private boolean optimizedAlgorithm(Coloring coloring, boolean proper, int maxColor, int index) {
        if (index >= vertices.length) {
            // We reached the end of the list
            return isCorrectlyColored(coloring);
        }

        Vertex v = vertices[index]; // We remove the vertex from the priority queue
        boolean[] colors = v.getAvailableColors();

        for (int color = 0; color < maxColor; color++) {
            if (!colors[color]) continue; // We skip this color as this can't be correct

            v.changeColor(color + 1); // + 1 as the actual colors are from 1 to n

            // We also change the available colors for the neighbours if the coloring is proper
            if (proper) {
                for (Vertex neighbour : v.getOpenNeighbourhood()) {
                    neighbour.removeColorFromAvailableColors(color);
                }
            }

            // We now order the remaining vertices
            Arrays.sort(vertices, index+1, vertices.length, Comparator.comparingInt(Vertex::getAmountOfAvailableColors));

            if (optimizedAlgorithm(coloring, proper, maxColor, index + 1)) {
                return true;
            }

            // We add back the available colors if it didn't work out
            if (proper) {
                for (Vertex neighbour : v.getOpenNeighbourhood()) {
                    neighbour.addColorFromAvailableColors(color);
                }
            }

        }

        return false;

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
        if (index >= vertices.length) {
            // We reached the end of the list
            return isCorrectlyColored(coloring);
        }

        for (int i = 1; i <= maxColor; i++) {
            vertices[index].changeColor(i);
            if (naiveAlgorithm(coloring, maxColor, index+1)) {
                return true;
            }
        }

        return false;

    }

}
