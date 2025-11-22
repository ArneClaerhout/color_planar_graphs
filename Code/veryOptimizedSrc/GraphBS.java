import java.util.ArrayList;
import java.util.Arrays;

public class GraphBS {

    /**
     * The array comprised of the vertices in this graph.
     * Each index in the array is an amount of available colors starting from 1.
     * So all vertices in the linked list at index 0, have 1 available color.
     */
    int[] vertices;

    private int filledColors = 0;

    /**
     * The indexed array that stays by index.
     */
    Vertex[] verticesIndexed;

    /**
     * A help-bitset for the verticesIndexed array that tracks which vertices have been colored.
     */
    private int vertexIsColored = 0;

    /**
     * A simple integer that keeps track of the amount of vertices.
     */
    private final int numberOfVertices;

    public GraphBS(String graph6) {
        char[] graphArray = graph6.toCharArray();
        this.numberOfVertices = getNumberOfVertices(graphArray);
        int[][] adjMatrix = getAdjacencyMatrix(graphArray, numberOfVertices);

        this.verticesIndexed = findVerticesList(adjMatrix);
        this.vertices = new int[10];
    }

    /**
     * A constructor for the graph class using the adjacency matrix of a graph.
     */
    public GraphBS(int[][] adjMatrix) {
        this.numberOfVertices = adjMatrix.length;
        this.verticesIndexed = findVerticesList(adjMatrix);
        this.vertices = new int[10];
    }

    /**
     * A method for coloring the graph,
     * this should only be used for testing the correctness of certain colorings.
     *
     * @param   colors
     *          The colors to assign to the vertices (with the correct vertices)
     */
    public void colorGraph(int[] colors) throws IllegalArgumentException {
        if (colors.length != vertices.length) {
            throw new IllegalArgumentException("Colors length is not equal to vertices length.");
        }
        for (int i = 0; i < vertices.length; i++) {
            if (colors[i] > 10 || colors[i] < 0) {
                throw new IllegalArgumentException("Colors are not in correct range.");
            }
            verticesIndexed[i].changeColor(colors[i]);
        }
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
     * A method for checking whether this graph
     * is correctly colored when using a given coloring method.
     * This method is private as we want to be able to choose
     * the proper check.
     *
     * @param   coloring
     *          The coloring method used.
     * @param   properLy
     *          Whether the fact that the vertex is proper should get checked.
     */
    private boolean isCorrectlyColored(Coloring coloring, boolean properLy) {
        // This method only gets called after coloring properLy
        if (!properLy && coloring == Coloring.PROPER) return true;

        for (Vertex v : verticesIndexed) {
            // It's important to give the indexed vertices.
            if (!v.isCorrectlyColored(coloring, verticesIndexed, properLy, false)) {
                return false;
            }
        }
        return true;
    }

    /**
     * A method for checking whether this graph is correctly colored when using a given coloring method.
     *
     * @param   coloring
     *          The coloring method used.
     */
    public boolean isCorrectlyColored(Coloring coloring) {
        return isCorrectlyColored(coloring, true);
    }

    /**
     * A method for getting the colors of the final graph, in the order of the adjacency matrix.
     *
     * @note    This method should only ever get used after coloring the graph.
     *          Otherwise, it will return nonsense.
     */
    public int[] getColors() {
        return Arrays.stream(verticesIndexed)
                .map(v -> v.getColor()).mapToInt(Integer::intValue).toArray();
        // We first make our vertices a stream
        // Then we sort them according to their indices
        // Afterward, we map all of our vertices to their respective color
        // To lastly convert the stream of Integers into an array of ints
    }


    /**
     * This method finds the number of vertices for a given graph6 formatted list of characters (= String).
     * This is a simplified version of this method that only works for graphs of up to 62 vertices.
     */
    private static int getNumberOfVertices(char[] graphString) {
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
    private static int[][] getAdjacencyMatrix(char[] graphString, int n) {
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
     * A different method for finding the adjacency matrix.
     * This method finds the matrix using the graph6 string itself.
     * @param   graph6
     *          The graph6 string to decode.
     */
    public static int[][] getAdjacencyMatrix(String graph6) {
        char[] graphArray = graph6.toCharArray();
        int n = getNumberOfVertices(graphArray);
        return getAdjacencyMatrix(graphArray, n);
    }

    /**
     * A method for creating a vertices array from a given adjacency matrix.
     *
     * @param   adjMatrix
     *          The adjacency matrix to find the vertices for.
     * @return  The array of vertices with each vertex instantiated.
     */
    private Vertex[] findVerticesList(int[][] adjMatrix) {
        Vertex[] vertices = new Vertex[numberOfVertices];

        for (int i = 0; i < numberOfVertices; i++) {
            vertices[i] = new Vertex(i);
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
        boolean um = Coloring.isUniqueMaximum(coloring);

        for (int i = 2; i <= n; i++) {
            // We don't forget to reset the vertices array
            vertices = new int[n];
            filledColors = (1 << (i - 1));
            for (Vertex v : verticesIndexed) {
                v.setMaxAvailableColors(i);
                // We add the vertices to the correct LL
                vertices[i - 1] |= 1 << v.getIndex();
            }
            // Each time we reset which vertices are colored.
            vertexIsColored = 0;
            if (optimizedAlgorithm(coloring, proper, um, 0, i)) {
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
     * @param   um
     *          Whether the coloring is a version of unique-maximum coloring.
     * @param   maxColorCurrGraph
     *          The current max color possible for this graph.
     *          This doesn't matter for unique-maximum colorings.
     * @param   maxColor
     *          The maximum color possible for this coloring method.
     * @return  True if the algorithm found a coloring for this graph.
     *          The colors of each of the vertex objects in vertices are the correct colors.
     *          False if there is no possible coloring for this maxColor.
     */
    private boolean optimizedAlgorithm(Coloring coloring, boolean proper, boolean um, int maxColorCurrGraph, int maxColor) {
        if (Integer.bitCount(vertexIsColored) == numberOfVertices) {
            return true;
        }

        int index = Integer.numberOfTrailingZeros(filledColors);
        int colV = vertices[index];

        int vertexIndex = Integer.numberOfTrailingZeros(colV);
        Vertex v = verticesIndexed[vertexIndex];

        vertices[index] &= ~(1 << vertexIndex);
        changeFilledColors(index, -1);
        // We remove this vertex from the available vertices

        int colors = v.getAvailableColors();

        int maxLoop = um ? maxColor : Math.min(maxColorCurrGraph + 1, maxColor);
        // Every coloring should be tried for um, as this is different for it.

        // We are coloring this index
        vertexIsColored |= 1 << vertexIndex;

        boolean neighboursColored = (v.getOpenNeighbourhood() & vertexIsColored) == v.getOpenNeighbourhood();

        for (int i = colors; i != 0; i &= i - 1) {
            int color = Integer.numberOfTrailingZeros(i);
            if (color > maxLoop) break; // We passed the highest possible color in the graph


            v.changeColor(color + 1); // + 1 as the actual colors are from 1 to n

            // We have to now check if all our neighbours are colored, as this isn't checked in updateNeighbours
            // This is an extra check for correctness
            if (neighboursColored && !v.isCorrectlyColored(coloring,  verticesIndexed, false, false)) {
                // This color isn't correct, we pick another
                continue;
            }

            // We also change the available colors for the neighbours if the coloring is proper
            ArrayList<Vertex> changed =  new ArrayList<>();

            if (updateNeighbours(v, color, coloring, changed)) {
                // This is used to skip this color, as it isn't possible
                continue;
            }

            int newMaxColorCurrGraph = Math.max(maxColorCurrGraph, color + 1);
            if (optimizedAlgorithm(coloring, proper, um, newMaxColorCurrGraph, maxColor)) {
                return true;
            }

            // We add back the available colors if it didn't work out
            if (proper) {
                changeBackVertices(changed, color);
            }

        }

        // We decolor this vertex
        vertexIsColored &= ~(1 << vertexIndex);
        v.changeColor(0);

        // We add the vertex back to be chosen.
        vertices[index] |= 1 << vertexIndex;
        changeFilledColors(-1, index);

        return false;

    }

//    /**
//     * A method for finding the best candidate in the coloring of a graph
//     *
//     * @param   index
//     *          The starting index in the vertices list.
//     *          All the vertices after (including the index itself)
//     *          are considered when finding the best candidate.
//     */
//    private int getBestIndex(int index) {
//        int bestIndex = index;
//        int smallestAC = vertices[bestIndex].getAmountOfAvailableColors();
//        if (smallestAC == 1) return index;
//        int testAC;
//        // We now order the remaining vertices, only for proper colorings
//        for (int i = index; i < vertices.length; i++) {
//            testAC = vertices[i].getAmountOfAvailableColors();
//            if (smallestAC > testAC) {
//                smallestAC = testAC;
//                bestIndex = i;
//            }
//        }
//        return bestIndex;
//    }

    /**
     * A method for updating the neighbours of a chosen vertex.
     * This method makes sure that only the actual real neighbours (in verticesIndexed) are changed.
     * It also only changes those neighbours that aren't colored yet.
     *
     * @param   v
     *          The vertex to update.
     * @param   color
     *          The color that should get added or removed
     *          from the neighbours available colors array.
     * @param   coloring
     *          The coloring to use, this is needed for the color-checking
     *          of neighbours while the algorithm is being run.
     * @param   changed
     *          This should be an empty list
     *          to be filled with the vertices that were changed.
     *          This will contain the old vertices that were removed from verticesIndexed.
     * @return  Whether we should skip this color
     *          as we already found a neighbour with zero possible colors.
     */
    private boolean updateNeighbours(Vertex v, int color, Coloring coloring, ArrayList<Vertex> changed) {
        boolean proper = Coloring.isProper(coloring);
        for (int i = v.getOpenNeighbourhood(); i != 0; i &= i - 1) {
            int bit = Integer.numberOfTrailingZeros(i);
            Vertex neighbour = verticesIndexed[bit];

            boolean neighbourIsColored = ((1 << bit) & vertexIsColored) > 0;

            if ((neighbour.getOpenNeighbourhood() & vertexIsColored) == neighbour.getOpenNeighbourhood() &&
                    neighbourIsColored) {
                // All the neighbour's neighbours are colored and the neighbour itself is colored
                // We want to check if the neighbour is CORRECTLY colored
                if (!neighbour.isCorrectlyColored(coloring, verticesIndexed, false, false)) {
                    // Early pruning
                    changeBackVertices(changed, color);
                    return true;
                    // We skip the rest, as this color is incorrect
                }
                // It doesn't have to be checked on whether it's proper, as this is done by the next section
            }

            if (proper) {
                // This section removes the color from the available colors

                // We check if the neighbour is already colored, as we don't have to do anything if this is the case
                if (neighbourIsColored) continue;

                // We compare the neighbours neighbourhood with the already colored vertices
                if (neighbour.removeColorFromAvailableColors(color)) {
                    int amountOfAvailableColors = neighbour.getAmountOfAvailableColors();// We removed one, but this is correct for the following indexing
                    if (amountOfAvailableColors != 0) {
                        changed.add(neighbour);
                        int amountOfAvaliableColors = neighbour.getAmountOfAvailableColors();
                        vertices[amountOfAvaliableColors] &= ~(1 << neighbour.getIndex());
                        vertices[amountOfAvaliableColors - 1] |= 1 << neighbour.getIndex();
                        changeFilledColors(amountOfAvaliableColors, amountOfAvaliableColors - 1);
                    } else {
                        // Early pruning
                        changeBackVertices(changed, color);
                        // The neighbour here isn't added to changed, we do this separately
                        neighbour.addColorFromAvailableColors(color);
                        return true;
                    }

                }

            }
        }
        return false;
    }

    /**
     * A method for changing back those updated by updateNeighbours.
     *
     * @param   changed
     *          The changed vertices to update back.
     * @param   color
     *          The color to add back to all the changed vertices.
     */
    private void changeBackVertices(ArrayList<Vertex> changed, int color) {
        for (Vertex changedNeighbour : changed) {
            // We add the colors back
            changedNeighbour.addColorFromAvailableColors(color);
            int amountOfAvaliableColors = changedNeighbour.getAmountOfAvailableColors();
            // We change back the neighbour LL's
            vertices[amountOfAvaliableColors - 2] &= ~(1 << changedNeighbour.getIndex());
            vertices[amountOfAvaliableColors - 1] |= 1 << changedNeighbour.getIndex();
            changeFilledColors(amountOfAvaliableColors - 2, amountOfAvaliableColors - 1);
        }
    }

    /**
     * A method for updating the filledColors variable.
     *
     * @param   oldIndex
     *          The old index of the updated vertex.
     *          This is the index in vertices where it gets removed from.
     * @param   newIndex
     *          The new index where the vertex is getting added into.
     */
    private void changeFilledColors(int oldIndex, int newIndex) {
        if (oldIndex != -1) {
            // Only if it doesn't get added back
            if (Integer.bitCount(vertices[oldIndex]) == 0) {
                // We removed the last vertex from here
                filledColors &= ~(1 << oldIndex);
            }
        }
        if (newIndex != -1) {
            if ((filledColors & (1 << newIndex)) == 0) {
                // It gets added to an empty list
                filledColors |= 1 << newIndex;
            }

        }
    }

}
