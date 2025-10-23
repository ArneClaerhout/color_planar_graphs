import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;

import static java.util.Collections.min;

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
     * @param   properLy
     *          Whether the fact that the vertex is proper should get checked.
     */
    public boolean isCorrectlyColored(Coloring coloring, boolean properLy) {
        for (Vertex v : vertices) {
            if (!v.isCorrectlyColored(coloring, properLy)) {
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

        for (int i = 2; i <= n; i++) {
            for (Vertex v : vertices) {
                v.setMaxAvailableColors(i);
            }
            if (optimizedAlgorithm(coloring, proper, 0, i, 0)) {
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
    private boolean optimizedAlgorithm(Coloring coloring, boolean proper, int maxColorRec, int maxColor, int index) {
        if (index >= vertices.length) {
            // We reached the end of the list
            if (coloring == Coloring.PROPER) return true; // We checked everything along the way, it should be correct
            return isCorrectlyColored(coloring, false);
            // If the coloring is proper, we have checked it already.
            // If the coloring is not proper, this shouldn't get checked.
        }

        Vertex v = vertices[index];
        boolean[] colors = v.getAvailableColors();


//        int maxLoop = (maxColorRec < maxColor) ? maxColorRec + 1 : maxColorRec;
        // TODO
        for (int color = 0; color < maxColor; color++) {
            if (!colors[color]) continue; // We skip this color as this can't be correct

            v.changeColor(color + 1); // + 1 as the actual colors are from 1 to n

            // We also change the available colors for the neighbours if the coloring is proper
            ArrayList<Vertex> changed =  new ArrayList<>();
            if (proper) {
                for (Vertex neighbour : v.getOpenNeighbourhood()) {
                    if (neighbour.removeColorFromAvailableColors(color)) changed.add(neighbour);
                    if (neighbour.getAmountOfAvailableColors() == 0) {
                        // Early pruning
                        for (Vertex changedNeighbour : changed) {
                            changedNeighbour.addColorFromAvailableColors(color);
                            // We add the colors back
                        }
                        return false;
                    }
                }

                // We make sure we are not at the last index
                if (index + 1 < vertices.length) {
                    // Instead of sorting the entire rest of the list, we find the best candidate
                    int bestIndex = getBestIndex(index + 1);

                    // We put the best starter at the front
                    if (index + 1 != bestIndex) {
                        Vertex tmp = vertices[bestIndex];
                        vertices[bestIndex] = vertices[index + 1];
                        vertices[index + 1] = tmp;
                    }
                }

            }


            int newMaxColorRec = Math.max(maxColorRec, color + 1); // +2 as the maxColorRec is not included in the loop
            if (optimizedAlgorithm(coloring, proper, newMaxColorRec, maxColor, index + 1)) {
                return true;
            }


            // We add back the available colors if it didn't work out
            if (proper) {
                for (Vertex neighbour : changed) {
                    neighbour.addColorFromAvailableColors(color);
                }
            }

        }

        return false;

    }

    /**
     * A method for finding the best candidate in the coloring of a graph
     *
     * @param   index
     *          The starting index in the vertices list.
     *          All the vertices after (including the index itself)
     *          are considered when finding the best candidate.
     */
    private int getBestIndex(int index) {
        int bestIndex = index;
        int smallestAC = vertices[bestIndex].getAmountOfAvailableColors();
        int testAC;
        // We now order the remaining vertices, only for proper colorings
        for (int i = index; i < vertices.length; i++) {
            testAC = vertices[i].getAmountOfAvailableColors();
            if (smallestAC > testAC) {
                smallestAC = testAC;
                bestIndex = i;
            }
        }
        return bestIndex;
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
            return isCorrectlyColored(coloring, true);
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
