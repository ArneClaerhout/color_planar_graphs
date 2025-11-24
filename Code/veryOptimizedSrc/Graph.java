import java.util.Arrays;

public class Graph {

//    /**
//     * The array comprised of the vertices in this graph.
//     */
//    Vertex[] vertices;

    /**
     * The indexed array that stays by index.
     */
    Vertex[] verticesIndexed;

    /**
     * A help-bitset for the verticesIndexed array that tracks which vertices have been colored.
     */
    private int vertexIsColored = 0;

    private int vertexIsAlmostColored = 0;

    /**
     * A help-bitset for checking whether the graph is almost fully colored.
     */
    private final int maxColoring;

    /**
     * A simple integer that keeps track of the amount of vertices.
     */
    private final int numberOfVertices;

    public Graph(String graph6) {
        char[] graphArray = graph6.toCharArray();
        this.numberOfVertices = getNumberOfVertices(graphArray);
        this.verticesIndexed = findFastVerticesList(graphArray, this.numberOfVertices);
        maxColoring = (1 << numberOfVertices) - 1;
        availables = maxColoring;
    }

    /**
     * A constructor for the graph class using the adjacency matrix of a graph.
     */
    public Graph(int[][] adjMatrix) {
        this.verticesIndexed = findVerticesList(adjMatrix, adjMatrix.length);
        this.numberOfVertices = adjMatrix.length;
        maxColoring = (1 << numberOfVertices) - 1;
        availables = maxColoring;
    }

    /**
     * A method for coloring the graph,
     * this should only be used for testing the correctness of certain colorings.
     *
     * @param   colors
     *          The colors to assign to the vertices (with the correct vertices)
     */
    public void colorGraph(int[] colors) throws IllegalArgumentException {
        if (colors.length != verticesIndexed.length) {
            throw new IllegalArgumentException("Colors length is not equal to vertices length.");
        }
        for (int i = 0; i < verticesIndexed.length; i++) {
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
     * A method for getting the vertices list directly from the graph string.
     *
     * @param   graphString
     *          The string in graph6 format which should be converted.
     * @param   n
     *          The amount of vertices for this graph.
     */
    private static Vertex[] findFastVerticesList(char[] graphString, int n) {
        Vertex[] vertices = new Vertex[n];

        for (int i = 0; i < n; i++) {
            vertices[i] = new Vertex(i);
        }
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
                    vertices[i].addNeighbour(vertices[j]);
                    // The matrix is symmetrical
                }

                bitPos++;
                if (bitPos % 6 == 0) {
                    index++;
                    bitPos = 0;
                }
            }
        }
        return vertices;

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
     * @param   n
     *          The amount of vertices.
     * @return  The array of vertices with each vertex instantiated.
     */
    private static Vertex[] findVerticesList(int[][] adjMatrix, int n) {
        Vertex[] vertices = new Vertex[n];

        for (int i = 0; i < n; i++) {
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
        boolean open = Coloring.isOpen(coloring);
        boolean um = Coloring.isUniqueMaximum(coloring);

        for (int i = 2; i <= n; i++) {
            for (Vertex v : verticesIndexed) {
                v.setMaxAvailableColors(i);
            }
            // Each time we reset which vertices are colored.
            vertexIsColored = 0;
            availables = maxColoring;
            if (optimizedAlgorithm(coloring, proper, um, open, 0, i, 0)) {
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
     * @param   open
     *          Whether the coloring is an open coloring,
     *          this also includes odd coloring.
     * @param   maxColorCurrGraph
     *          The current max color possible for this graph.
     *          This doesn't matter for unique-maximum colorings.
     * @param   maxColor
     *          The maximum color possible for this coloring method.
     * @param   index
     *          The index of where this algorithm is working.
     *
     * @return  True if the algorithm found a coloring for this graph.
     *          The colors of each of the vertex objects in vertices are the correct colors.
     *          False if there is no possible coloring for this maxColor.
     */
    private boolean optimizedAlgorithm(Coloring coloring, boolean proper, boolean um, boolean open, int maxColorCurrGraph, int maxColor, int index) {
        if (vertexIsColored == maxColoring) {
            // We reached the end
            return true;
        }

        Vertex v = verticesIndexed[index];
        int colors = v.getAvailableColors();

        int maxLoop = um ? maxColor : Math.min(maxColorCurrGraph + 1, maxColor);
        // Every coloring should be tried for um, as this is different for it.

        // We are coloring this index
        vertexIsColored |= 1 << index;
//        vertexIsAlmostColored &= ~(1 << index);

        boolean lastToColor = (maxColoring & ~vertexIsColored) == 0;

        int neighbourhood = v.getOpenNeighbourhood();
        boolean neighboursColored = (neighbourhood & vertexIsColored) == neighbourhood;

        for (int i = colors; i != 0; i &= i - 1) {
            int color = Integer.numberOfTrailingZeros(i);
            if (color > maxLoop) break; // We passed the highest possible color in the graph
            if (lastToColor && maxColorCurrGraph < maxColor && color <= maxColorCurrGraph) continue; // We don't want to retry already tried states

            v.changeColor(color + 1); // + 1 as the actual colors are from 1 to n

            // We have to now check if all our neighbours are colored, as this isn't checked in updateNeighbours
            // This is an extra check for correctness
            if (neighboursColored && !v.isCorrectlyColored(coloring, verticesIndexed, false, false)) {
                // This color isn't correct, we pick another
                continue;
            }
            // We also change the available colors for the neighbours if the coloring is proper
            int[] changed = new int[numberOfVertices];

            if (updateNeighbours(v, color, coloring, open, changed, proper)) {
                // This is used to skip this color, as it isn't possible
                continue;
            }

            int newMaxColorCurrGraph = Math.max(maxColorCurrGraph, color + 1);
            int newIndex = getBestIndex(index);
            if (optimizedAlgorithm(coloring, proper, um, open, newMaxColorCurrGraph, maxColor, newIndex)) {
                return true;
            }

            // We add back the available colors if it didn't work out
            addColorsBack(changed);
        }
        // We decolor this vertex
        vertexIsColored &= ~(1 << index);
        v.changeColor(0);
        availables |= (1 << index);

//        for (int i = v.getOpenNeighbourhood(); i != 0; i &= i - 1) {
//            verticesIndexed[Integer.numberOfTrailingZeros(i)].decrementAmountOfColoredNeighbours();
//        }

        return false;

    }

    /**
     * A bit-set keeping track of which vertices are still able to be colored.
     *
     * For a naïve coloring chooser, this will be the opposite of maxColoring.
     * For more complex choosers, this is different.
     */
    private int availables;

    /**
     * A method for finding the best candidate in the coloring of a graph.
     */
    private int getBestIndex(int indexColored) {
        // The addition of tiebreaks with degree only slows it down (this is done in DSATUR),
        //  taking the first best vertex is fastest
        if (indexColored != -1) availables &= ~(1 << indexColored);
        int bestIndex = 0;
        int smallestAC = Integer.MAX_VALUE;
        int testAC;
        for (int i = availables; i != 0; i &= i - 1) {
            int index = Integer.numberOfTrailingZeros(i);
            testAC = verticesIndexed[index].getAmountOfAvailableColors();
            if (smallestAC > testAC) {
                smallestAC = testAC;
                bestIndex = index;
            }
            if (smallestAC == 1) return bestIndex; // We break early as this is the best possible
        }
        return bestIndex;
    }

    /**
     * A method for finding the best candidate in the coloring of a graph.
     *
     * This best-ndex method doesn't work well
     * as keeping track of the amount of colored neighbours
     * already adds a lot of overhead,
     * which isn't compensated by good vertex choosing.
     */
    private int getBestIndex2(int indexColored) {
        if (indexColored != -1) availables &= ~(1 << indexColored);
        int bestIndex = 0;
        int smallestDiff = Integer.MAX_VALUE;
        int testDiff;
        for (int i = availables; i != 0; i &= i - 1) {
            int index = Integer.numberOfTrailingZeros(i);
            testDiff = verticesIndexed[index].getDegree() - verticesIndexed[index].getAmountOfColoredNeighbours();
            if (smallestDiff > testDiff) {
                smallestDiff = testDiff;
                bestIndex = index;
            }
            if (smallestDiff == 1) return bestIndex; // We break early as this is the best possible
        }
        return bestIndex;
    }

    /**
     * A method for finding the best candidate in the coloring of a graph.
     */
    private int getBestIndex3(int indexColored) {
        if (indexColored != -1) availables &= ~(1 << indexColored);
        int bestIndex = 0;
        int smallestDiff = Integer.MAX_VALUE;
        int testDiff;
        for (int i = availables; i != 0; i &= i - 1) {
            int index = Integer.numberOfTrailingZeros(i);
            testDiff = verticesIndexed[index].getImportanceValue();
            if (smallestDiff > testDiff) {
                smallestDiff = testDiff;
                bestIndex = index;
            }
            if (smallestDiff == 1) return bestIndex; // We break early as this is the best possible
        }
        return bestIndex;
    }

    /**
     * A method for finding the best candidate in the coloring of a graph.
     */
    private int getBestIndexWP(int indexColored) {
        if (indexColored != -1) availables &= ~(verticesIndexed[indexColored].getOpenNeighbourhood() | (1 << indexColored));

        // If this removes all vertices, reset
        if (availables == 0) availables = maxColoring & ~vertexIsColored;

        int maxDegree = 0;
        int maxDegreeIndex = 0;
        for (int i = availables; i != 0; i &= i - 1) {
            int index = Integer.numberOfTrailingZeros(i);
            int degree = verticesIndexed[index].getDegree();
            if (degree > maxDegree) {
                maxDegree = degree;
                maxDegreeIndex = index;
            }
        }

        return maxDegreeIndex;
    }

    /**
     * A method for finding the best candidate in the coloring of a graph.
     */
    private int getBestIndexWP2(int indexColored) {
        if (indexColored != -1) availables &= ~(verticesIndexed[indexColored].getOpenNeighbourhood() | (1 << indexColored));

        // If this removes all vertices, reset
        if (availables == 0) availables = maxColoring & ~vertexIsColored;

        int minAC = Integer.MAX_VALUE;
        int minACIndex = 0;
        int minACDegree = 0;
        Vertex testV;
        int index;
        for (int i = availables; i != 0; i &= i - 1) {
            index = Integer.numberOfTrailingZeros(i);
            testV = verticesIndexed[index];
            int testAC = testV.getAmountOfAvailableColors();
            if (minAC > testAC) {
                minAC = testAC;
                minACIndex = index;
                minACDegree = testV.getDegree();
            } else if (minAC == testAC) {
                int testDegree = testV.getDegree();
                if (minACDegree < testDegree) {
                    minACIndex = index;
                    minACDegree = testDegree;
                }
            }
        }

        return minACIndex;
    }

    /**
     * A method for finding the best candidate in the coloring of a graph.
     */
    private int getBestIndexDSATUR(int indexColored) {
        if (indexColored != -1) availables &= ~(1 << indexColored);

        int minAC = Integer.MAX_VALUE;
        int minACIndex = 0;
        int minACDegree = 0;
        Vertex testV;
        int index;
        for (int i = availables; i != 0; i &= i - 1) {
            index = Integer.numberOfTrailingZeros(i);
            testV = verticesIndexed[index];
            int testAC = testV.getAmountOfAvailableColors();
            if (minAC > testAC) {
                minAC = testAC;
                minACIndex = index;
                minACDegree = testV.getDegree();
            } else if (minAC == testAC) {
                int testDegree = testV.getDegree();
                if (minACDegree < testDegree) {
                    minACIndex = index;
                    minACDegree = testDegree;
                }
            }
        }

        return minACIndex;
    }

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
     * @param   open
     *          Whether the coloring is an open coloring,
     *          this also includes odd coloring.
     * @param   changed
     *          This should be an empty list
     *          to be filled with the vertices that were changed.
     *          This will contain the old vertices that were removed from verticesIndexed.
     * @param   proper
     *          Whether the coloring is proper.
     *
     * @return  True if this color should get skipped
     *          as we already found a neighbour with zero possible colors.
     *          False otherwise.
     */
    private boolean updateNeighbours(Vertex v, int color, Coloring coloring, boolean open, int[] changed, boolean proper) {
        for (int i = v.getOpenNeighbourhood(); i != 0; i &= i - 1) {
            int bit = Integer.numberOfTrailingZeros(i);
            Vertex neighbour = verticesIndexed[bit];

//            neighbour.incrementAmountOfColoredNeighbours();

            boolean neighbourIsColored = ((1 << bit) & vertexIsColored) != 0;

            int neighbourhood = neighbour.getOpenNeighbourhood();
            neighbourhood = (open ? neighbourhood : (neighbourhood | (1 << bit)));

            int diff = neighbourhood & ~vertexIsColored;

            // We will allow adding vertices with one possible color that aren't colored
//            diff &= ~vertexIsAlmostColored;

            if (diff == 0 && (open || neighbourIsColored)) {
                // All the neighbour's neighbours are colored and the neighbour itself is colored
                // We want to check if the neighbour is CORRECTLY colored
                if (!neighbour.isCorrectlyColored(coloring, verticesIndexed, false, true)) {
                    // Early pruning
                    addColorsBack(changed);
                    return true;
                    // We skip the rest, as this color is incorrect
                }
                // It doesn't have to be checked on whether it's proper, as this is done by the next section
            }
            else if (diff != 0 && (diff & (diff - 1)) == 0) { // bitCount(diff) == 1
                int toColorNeighbourIndex = Integer.numberOfTrailingZeros(diff);
                Vertex toColorNeighbour = verticesIndexed[toColorNeighbourIndex];
                // There's one vertex that isn't colored yet.
                if (Coloring.isConflictFree(coloring) && (neighbourIsColored || !proper) ) {
                    if (handleCF(changed, toColorNeighbour, toColorNeighbourIndex, neighbourhood)) {
                        return true;
                    }

                } else if (Coloring.isUniqueMaximum(coloring) && (neighbourIsColored || !proper)) {
                    if (handleUM(changed, toColorNeighbour, toColorNeighbourIndex, neighbourhood)) {
                        return true;
                    }

                } else if (coloring == Coloring.ODD && neighbourIsColored) {
                    // It shouldn't be the neighbour itself,
                    // as otherwise we can do the proper update
                    if (handleOdd(changed, toColorNeighbour, toColorNeighbourIndex, neighbourhood)) {
                        return true;
                    }
                }
                // Proper colorings are done later
            }

            if (proper) {
                // This section removes the color from the available colors

                // We check if the neighbour is already colored, as we don't have to do anything if this is the case
                if (neighbourIsColored) continue;

                // We compare the neighbours neighbourhood with the already colored vertices
                if (neighbour.removeColorFromAvailableColors(color)) {
                    changed[bit] |= (1 << color);
                    if (neighbour.getAmountOfAvailableColors() == 0) {
                        // Early pruning
                        addColorsBack(changed);
                        return true;
                    }
//                    else if (neighbour.getAmountOfAvailableColors() == 1) {
//                        vertexIsAlmostColored |= (1 << bit);
//                    }
                }
            }
        }
        return false;
    }

    /**
     * A method for adding the colors back to changed vertices.
     *
     * @param   changed
     *          The changed array containing the vertices changed,
     *          paired with the colors that were removed (and thus have to get added back).
     */
    private void addColorsBack(int[] changed) {
        for (int i = 0; i < changed.length; i++) {
            int value = changed[i];
            if (value != 0) {
                Vertex changedNeighbour = verticesIndexed[i];
                while (value != 0) {
                    int valueIndex = Integer.numberOfTrailingZeros(value);
                    value &= value - 1; // clear the lowest set bit
                    changedNeighbour.addColorFromAvailableColors(valueIndex);
                }
//                if (changedNeighbour.getAmountOfAvailableColors() == 1) {
//                    vertexIsAlmostColored |= 1 << changedNeighbour.getIndex();
//                } else {
//                    vertexIsAlmostColored &= ~(1 << changedNeighbour.getIndex());
//                }
            }
        }
    }

    /**
     * A helper method for handling the Conflict-Free coloring part of the optimisation.
     */
    private boolean handleCF(int[] changed, Vertex toColorNeighbour, int toColorNeighbourIndex, int neighbourhood) {
        int colorsOccurOnce = 0;
        int colorsOccur = 0;

        Vertex secondNeighbour;
        int colorIndex;

        neighbourhood = neighbourhood & ~(1 << toColorNeighbourIndex);
        for (int i = neighbourhood; i != 0; i &= i - 1) {
            int index = Integer.numberOfTrailingZeros(i);
            secondNeighbour = verticesIndexed[index];
            int neighbourColor = secondNeighbour.getColor();
            // We do -1 as the colors are from 1...k,
            // but we want to later on use the colors 0...k-1

            if (neighbourColor == 0) { // We now for a fact that this vertex has one color available, as this was checked beforehand
                colorIndex = 1 << Integer.numberOfTrailingZeros(secondNeighbour.getAvailableColors());
            } else {
                colorIndex = 1 << (neighbourColor - 1);
            }

            if ((colorsOccur & colorIndex) != 0) {
                colorsOccurOnce &= ~colorIndex;
            } else {
                colorsOccurOnce |= colorIndex;
                colorsOccur |= colorIndex;
            }
        }

        if (colorsOccurOnce == 0) {
            for (int j = colorsOccur; j != 0; j &= j - 1) {
                int bit = Integer.numberOfTrailingZeros(j);
                if (removeColor(toColorNeighbour, toColorNeighbourIndex, bit, changed)) return true;
            }
        } else if ((colorsOccurOnce & (colorsOccurOnce - 1)) == 0) { // bitCount == 1
            int j = Integer.numberOfTrailingZeros(colorsOccurOnce);
            if (removeColor(toColorNeighbour, toColorNeighbourIndex, j, changed)) return true;
        }
        return false;
    }

    /**
     * A helper method for handling the Unique-Maximum coloring part of the optimisation.
     */
    private boolean handleUM(int[] changed, Vertex toColorNeighbour, int toColorNeighbourIndex, int neighbourhood) {
        int max = 1;
        int amountOfMax = 0;

        Vertex secondNeighbour;

        neighbourhood = neighbourhood & ~(1 << toColorNeighbourIndex);
        for (int i = neighbourhood; i != 0; i &= i - 1) {
            int index = Integer.numberOfTrailingZeros(i);
            secondNeighbour = verticesIndexed[index];
            int neighbourColor = secondNeighbour.getColor();

            if (neighbourColor == 0) { // We now for a fact that this vertex has one color available, as this was checked beforehand
                neighbourColor = Integer.numberOfTrailingZeros(secondNeighbour.getAvailableColors()) + 1;
            }

            if (neighbourColor == max) {
                amountOfMax++;
            } else if (neighbourColor > max) {
                max = neighbourColor;
                amountOfMax = 1;
            }
        }

        // We need to find the maximum and the amount of times it occurs
        // If it occurs once, don't use this color, if it occurs more than once
        //  Only allow colors bigger than the max
        // Otherwise, do nothing.
        if (amountOfMax == 1) {
            if (removeColor(toColorNeighbour, toColorNeighbourIndex, max - 1, changed)) return true;
        } else if (amountOfMax > 1) {
            for (int colorToRemove = 0; colorToRemove < max; colorToRemove++) {
                if (removeColor(toColorNeighbour, toColorNeighbourIndex, colorToRemove, changed)) return true;
            }
        }
        return false;
    }

    /**
     * A helper method for handling the Odd coloring part of the optimisation.
     */
    private boolean handleOdd(int[] changed, Vertex toColorNeighbour, int toColorNeighbourIndex, int neighbourhood) {
        int colorsOccurOdd = 0;
        Vertex secondNeighbour;

        neighbourhood = neighbourhood & ~(1 << toColorNeighbourIndex);
        for (int i = neighbourhood; i != 0; i &= i - 1) {
            int index = Integer.numberOfTrailingZeros(i);
            secondNeighbour = verticesIndexed[index];
            int neighbourColor = secondNeighbour.getColor();

            if (neighbourColor == 0) { // We now for a fact that this vertex has one color available, as this was checked beforehand
                colorsOccurOdd ^= (1 << (Integer.numberOfTrailingZeros(secondNeighbour.getAvailableColors())));
            } else {
                colorsOccurOdd ^= (1 << (neighbourColor - 1));
                // We do -1 as the colors are from 1...k,
                // but we want to later on use the colors 0...k-1
            }
        }

        if (colorsOccurOdd != 0 && (colorsOccurOdd & (colorsOccurOdd - 1)) == 0) { // Check whether there's one bit
            // Don't take this color
            int index = Integer.numberOfTrailingZeros(colorsOccurOdd);
            if (removeColor(toColorNeighbour, toColorNeighbourIndex, index, changed)) return true;
        }
        return false;
    }

    /**
     * A method for removing the color from the available colors,
     * while also checking to see if the vertex has 0 colors left.
     * If this is the case,
     * it will add all the colors back to all vertices in changed (using addColorsBack).
     *
     * @param   v
     *          The vertex to remove the colors from.
     * @param   index
     *          The index of the given vertex.
     * @param   color
     *          The color to remove from the vertex.
     * @param   changed
     *          The changed list,
     *          containing all the vertices that were already updated.
     *
     * @return  True if we can prune, meaning there was a vertex with no available colors.
     *          False otherwise.
     */
    private boolean removeColor(Vertex v, int index, int color, int[] changed) {
        if (!v.removeColorFromAvailableColors(color)) return false;
        changed[index] |= (1 << color);
        int m = v.getAmountOfAvailableColors();
        if (m == 0) {
            addColorsBack(changed);
            return true;
        }
//        else if (m == 1) {
//            vertexIsAlmostColored |= (1 << index);
//        }
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
        if (index >= verticesIndexed.length) {
            // We reached the end of the list
            return isCorrectlyColored(coloring, true);
        }

        for (int i = 1; i <= maxColor; i++) {
            verticesIndexed[index].changeColor(i);
            if (naiveAlgorithm(coloring, maxColor, index+1)) {
                return true;
            }
        }

        return false;

    }

}
