import java.util.*;
import java.util.stream.Stream;

import static java.util.stream.Collectors.toList;

public class GraphPQ {

    /**
     * The Priority Queue comprised of the vertices in this graph.
     * Sorted by the amount of available colors left.
     */
    private PriorityQueue<VertexPQ> vertices = new PriorityQueue<>(Comparator.comparingInt(VertexPQ::getAmountOfAvailableColors));

    private final VertexPQ[] verticesIndexed;

    private int vertexIsColored = 0;

    public GraphPQ(String graph6) {
        char[] graphArray = graph6.toCharArray();
        int n = getNumberOfVertices(graphArray);
        int[][] adjMatrix = getAdjacencyMatrix(graphArray, n);

        this.verticesIndexed = findVerticesList(adjMatrix, n);
        vertices.addAll(List.of(this.verticesIndexed));
    }

    /**
     * A constructor for the graph class using the adjacency matrix of a graph.
     */
    public GraphPQ(int[][] adjMatrix) {
        int n = adjMatrix.length;
        this.verticesIndexed = findVerticesList(adjMatrix, n);
        vertices.addAll(List.of(this.verticesIndexed));
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
     * @param   vertices
     *          The vertices to check this property on.
     */
    private boolean isCorrectlyColored(Coloring coloring, boolean properLy, VertexPQ[] vertices) {
        // We know that proper colorings are correct when this method is called
        if (!properLy && coloring == Coloring.PROPER) return true;

        for (VertexPQ v : vertices) {
            if (!v.isCorrectlyColored(coloring, this.verticesIndexed, properLy)) {
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
     * @param   vertices
     *          The vertices to check this property on.
     */
    public boolean isCorrectlyColored(Coloring coloring, VertexPQ[] vertices) {
        return isCorrectlyColored(coloring, true, vertices);
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
        // We don't have to sort them according to indices
        // Afterward, we map all of our vertices to their respective color
        // To lastly convert the stream of Integers into an array of ints
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
    private VertexPQ[] findVerticesList(int[][] adjMatrix, int n) {
        VertexPQ[] vertices =  new VertexPQ[n];

        for (int i = 0; i < n; i++) {
            vertices[i] = (new VertexPQ(i, n));
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
            vertexIsColored = 0;
            vertices.clear();
            vertices.addAll(List.of(this.verticesIndexed));
            for (VertexPQ v : vertices) {
                v.setMaxAvailableColors(i);
                // Note that this changes the amount of available colors
                // This does not matter for the PQ as they're all the same value
            }
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
    private boolean optimizedAlgorithm(Coloring coloring, boolean proper, boolean um,
                                       int maxColorCurrGraph, int maxColor) {
        if (vertices.isEmpty() || Integer.bitCount(vertexIsColored) == verticesIndexed.length) {
            // We check the coloring
            return true;

        }

        for (VertexPQ v : verticesIndexed) {
            if ((vertexIsColored & (1 << v.getIndex())) == 0 && !vertices.contains(v)) {
                System.out.println(v.getIndex());
            }
        }

        VertexPQ v = vertices.poll();

        // The second check is to see if this vertex is real
        while (!vertices.isEmpty() &&
                ((vertexIsColored & (1 << v.getIndex())) > 0 || v != (verticesIndexed[v.getIndex()])) )
            v = vertices.poll();

        int vertexIndex = v.getIndex(); // Actual index

        // We either end the loop if the vertices are empty or if the vertex is real
        // We have to recheck whether we ended on the case where the last vertex is not real
        // (If the last vertex is real, we just do another loop of the algorithm)
        if (vertices.isEmpty() && v != (verticesIndexed[v.getIndex()])) {
            // We check the coloring
            return true;
        }

        boolean[] colors = v.getAvailableColors();

        int maxLoop = um ? maxColor : Math.min(maxColorCurrGraph + 1, maxColor);
        // Every coloring should be tried for um, as this is different for it.

        // We are coloring this index
        vertexIsColored |= 1 << vertexIndex;

        boolean neighboursColored = (v.getOpenNeighbourhood() & vertexIsColored) == v.getOpenNeighbourhood();

        for (int color = 0; color < maxLoop; color++) {
            if (!colors[color]) continue; // We skip this color as this can't be correct

            v.changeColor(color + 1); // + 1 as the actual colors are from 1 to n

            // We have to now check if all our neighbours are colored, as this isn't checked in updateNeighbours
            // This is an extra check for correctness
            if (neighboursColored && !v.isCorrectlyColored(coloring,  verticesIndexed, false)) {
                // This color isn't correct, we pick another
                continue;
            }

            // We create the changed ArrayList for this vertex here, as we can use this later on
            ArrayList<VertexPQ> changed = new ArrayList<>();


            // We also change the available colors for the neighbours if the coloring is proper
            if (updateNeighbours(v.getOpenNeighbourhood(), coloring, color, false, changed)) {
                // We enter this if statement when we find an early prune, found by updateNeighbours
                continue;
            }


            int newMaxColorCurrGraph = Math.max(maxColorCurrGraph, color + 1);
            if (optimizedAlgorithm(coloring, proper, um, newMaxColorCurrGraph, maxColor)) {
                return true;
            }


            // We add back the available colors if it didn't work out
            if (proper) {
                updateNeighbours(v.getOpenNeighbourhood(), coloring, color, true, changed);
            }

        }

        // We add the vertex back to be chosen
        vertices.add(v);
        v.changeColor(0);
        vertexIsColored &= ~(1 << vertexIndex);

        return false;

    }

    /**
     * A method for updating the neighbours of a chosen vertex.
     * This method makes sure that only the actual real neighbours (in verticesIndexed) are changed.
     * It also only changes those neighbours that aren't colored yet.
     *
     * @param   neighbourhood
     *          The neighbourhood to update.
     * @param   coloring
     *          The coloring used.
     * @param   color
     *          The color that should get added or removed
     *          from the neighbours available colors array.
     * @param   addColor
     *          Whether the color should get added back to the neighbours.
     *          This only does it for the neighbours given in the ArrayList changed.
     * @param   changed
     *          When addColor is true, these are the neighbours
     *          already changed before when removing the color.
     *          When addColor is false, this should be an empty list
     *          to be filled with the vertices that were changed.
     *          This will contain the old vertices that were removed from verticesIndexed.
     * @return  Whether we should skip this color
     *          as we already found a neighbour with zero possible colors.
     */
    private boolean updateNeighbours(int neighbourhood, Coloring coloring, int color, boolean addColor, ArrayList<VertexPQ> changed) {
        if (addColor) {
            // We need to undo everything in changed
            for (VertexPQ changedNeighbour : changed) {
                // This readds the changed neighbours to the actual vertices array
                verticesIndexed[changedNeighbour.getIndex()] = changedNeighbour;
                vertices.add(changedNeighbour);
                // We also readd the changedNeighbours back as they could have been removed already.
            }
            return false; // This doesn't matter as we don't use this in this context
        }

        boolean proper = Coloring.isProper(coloring);

        for (int i = neighbourhood; i != 0; i &= i - 1) {
            int bit = Integer.numberOfTrailingZeros(i);
            VertexPQ neighbour = verticesIndexed[bit];

            boolean neighbourIsColored = ((1 << bit) & vertexIsColored) > 0;

            if ((neighbour.getOpenNeighbourhood() & vertexIsColored) == neighbour.getOpenNeighbourhood() &&
                    neighbourIsColored) {
                // All the neighbour's neighbours are colored and the neighbour itself is colored
                // We want to check if the neighbour is CORRECTLY colored
                if (!neighbour.isCorrectlyColored(coloring, verticesIndexed, false)) {
                    // Early pruning
                    for (VertexPQ changedNeighbour : changed) {
                        // This readds the changed neighbours to the actual vertices array
                        verticesIndexed[changedNeighbour.getIndex()] = changedNeighbour;
                    }
                    return true;
                    // We skip the rest, as this color is incorrect
                }
                // It doesn't have to be checked on whether it's proper, as this is done by the next section
            }


            if (proper) {
                if (neighbourIsColored) continue;
                // The old neighbour is invalidated, we add the new one
                VertexPQ newNeighbour = new VertexPQ(neighbour);

                // We remove the colors and check for early pruning
                if (newNeighbour.removeColorFromAvailableColors(color)) {
                    if (newNeighbour.getAmountOfAvailableColors() == 0) {
                        for (VertexPQ changedNeighbour : changed) {
                            // This readds the changed neighbours to the actual vertices array
                            verticesIndexed[changedNeighbour.getIndex()] = changedNeighbour;
                        }
                        return true;
                    }
                    verticesIndexed[newNeighbour.getIndex()] = newNeighbour;
                    vertices.add(newNeighbour);

                    // We keep track of the new and old neighbours
                    changed.add(neighbour);
                }
            }
        }
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

//
//    public int findChromaticNumberNaive(Coloring coloring) {
//        int n = coloring.getMaxChromaticNumber();
//
//        for (int i = 1; i <= n; i++) {
//            if (naiveAlgorithm(coloring, i, 0)) {
//                return i;
//            }
//        }
//        return 0;
//    }

//    private boolean naiveAlgorithm(Coloring coloring, int maxColor, int index) {
//        if (index >= vertices.length) {
//            // We reached the end of the list
//            return isCorrectlyColored(coloring, true);
//        }
//
//        for (int i = 1; i <= maxColor; i++) {
//            vertices[index].changeColor(i);
//            if (naiveAlgorithm(coloring, maxColor, index+1)) {
//                return true;
//            }
//        }
//
//        return false;
//
//    }

}
