import java.util.ArrayList;
import java.util.Arrays;

public class GraphBS extends Graph {

    /**
     * The array comprised of the vertices in this graph.
     * Each index in the array is an amount of available colors starting from 1.
     * So all vertices in the linked list at index 0, have 1 available color.
     */
    int[] vertices;

    private int filledColors = 0;

    public GraphBS(String graph6) {
        super(graph6);
    }

    /**
     * A constructor for the graph class using the adjacency matrix of a graph.
     */
    public GraphBS(int[][] adjMatrix) {
        super(adjMatrix);
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int findChromaticNumberOptimized(Coloring coloring, boolean open, boolean proper, boolean um, boolean checkCondition, boolean allColorings) {
        int n = coloring.getMaxChromaticNumber();

        for (int i = 2; i <= n; i++) {
            // We don't forget to reset the vertices array
            vertices = new int[n];
            filledColors = (1 << (i - 1));
            for (Vertex v : verticesIndexed) {
                v.setMaxAvailableColors(i);
                // We add the vertices to the correct LL
            }
            vertices[i - 1] = maxColoring;
            // Each time we reset which vertices are colored.
            vertexIsColored = 0;
            if (optimizedAlgorithm(coloring, open, proper, um, 0, i, checkCondition, allColorings)) {
                return i;
            }
            if (chromaticNumber != 0) return i;
        }
        return 0;
    }

    /**
     * A method for running the optimized algorithm using the memorization of available colors for each vertex.
     *
     * @param   coloring
     *          The coloring of which the algorithm should to try to find a solution for.
     * @param   open
     *          Whether the coloring is an open coloring,
     *          this also includes odd coloring.µ
     * @param   proper
     *          Whether the coloring is proper.
     * @param   um
     *          Whether the coloring is a version of unique-maximum coloring.
     * @param   maxColorCurrGraph
     *          The current max color possible for this graph.
     *          This doesn't matter for unique-maximum colorings.
     * @param   maxColor
     *          The maximum color possible for this coloring method.
     * @param   checkCondition
     *          True if a condition, specified in the Counter class, should get checked.
     *          False otherwise.
     * @param   allColorings
     *          True if all colorings for a given graph should get found.
     *
     * @return  True if the algorithm found a coloring for this graph.
     *          The colors of each of the vertex objects in vertices are the correct colors.
     *          False if there is no possible coloring for this maxColor.
     */
    private boolean optimizedAlgorithm(Coloring coloring, boolean open, boolean proper, boolean um, int maxColorCurrGraph, int maxColor, boolean checkCondition, boolean allColorings) {
        if (vertexIsColored == maxColoring) {
            return startingStep(maxColor, checkCondition, allColorings);
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
            if (neighboursColored && !v.isCorrectlyColored(coloring,  verticesIndexed, false, false, open, proper, um)) {
                // This color isn't correct, we pick another
                continue;
            }

            // We also change the available colors for the neighbours if the coloring is proper
            ArrayList<Vertex> changed =  new ArrayList<>();

            if (updateNeighbours(v, color, coloring, open, proper, um, changed)) {
                // This is used to skip this color, as it isn't possible
                continue;
            }

            int newMaxColorCurrGraph = Math.max(maxColorCurrGraph, color + 1);
            if (optimizedAlgorithm(coloring, open, proper, um, newMaxColorCurrGraph, maxColor, checkCondition, allColorings)) {
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
     *          this also includes odd coloring.µ
     * @param   proper
     *          Whether the coloring is proper.
     * @param   um
     *          Whether the coloring is a unique-maximum coloring.
     * @param   changed
     *          This should be an empty list
     *          to be filled with the vertices that were changed.
     *          This will contain the old vertices that were removed from verticesIndexed.
     * @return  Whether we should skip this color
     *          as we already found a neighbour with zero possible colors.
     */
    private boolean updateNeighbours(Vertex v, int color, Coloring coloring, boolean open, boolean proper, boolean um, ArrayList<Vertex> changed) {
        for (int i = v.getOpenNeighbourhood(); i != 0; i &= i - 1) {
            int bit = Integer.numberOfTrailingZeros(i);
            Vertex neighbour = verticesIndexed[bit];

            boolean neighbourIsColored = ((1 << bit) & vertexIsColored) > 0;

            if ((neighbour.getOpenNeighbourhood() & vertexIsColored) == neighbour.getOpenNeighbourhood() &&
                    neighbourIsColored) {
                // All the neighbour's neighbours are colored and the neighbour itself is colored
                // We want to check if the neighbour is CORRECTLY colored
                if (!neighbour.isCorrectlyColored(coloring, verticesIndexed, false, false, open, proper, um)) {
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
