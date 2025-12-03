import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.NoSuchElementException;

public class GraphLL extends Graph {

    /**
     * The array comprised of the vertices in this graph.
     * Each index in the array is an amount of available colors starting from 1.
     * So all vertices in the linked list at index 0, have 1 available color.
     */
    VertexLL[] vertices;

    /**
     * The indexed array that stays by index.
     */
    VertexLL[] verticesIndexed;

    public GraphLL(String graph6) {
        super(graph6);
        this.verticesIndexed = Arrays.stream(super.verticesIndexed).map(VertexLL::new).toArray(VertexLL[]::new);
        this.vertices = new VertexLL[10];
    }

    /**
     * A constructor for the graph class using the adjacency matrix of a graph.
     */
    public GraphLL(int[][] adjMatrix) {
        super(adjMatrix);
        this.verticesIndexed = (VertexLL []) super.verticesIndexed;
        this.vertices = new VertexLL[10];
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int findChromaticNumberOptimized(Coloring coloring, boolean open, boolean proper, boolean um, boolean checkCondition, boolean allColorings) {
        int n = coloring.getMaxChromaticNumber();

        for (int i = 2; i <= n; i++) {
            // We don't forget to reset the vertices array
            vertices = new VertexLL[n];
            for (VertexLL v : verticesIndexed) {
                v.setMaxAvailableColors(i);
                // We add the vertices to the correct LL
                v.resetLL();
                v.addToLL(vertices, i - 1);
            }
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
     *          this also includes odd coloring.
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
    private boolean optimizedAlgorithm(Coloring coloring, boolean open, boolean proper, boolean um,
                                       int maxColorCurrGraph, int maxColor, boolean checkCondition, boolean allColorings) {
        if (vertexIsColored == maxColoring) {
            return startingStep(maxColor, checkCondition, allColorings);
        }

        int index = 0;
        VertexLL v = vertices[index];
        while (v == null) {
            index++;
            v = vertices[index];
        }
        v.removeFromLL(vertices, v.getAmountOfAvailableColors() - 1);
        int vertexIndex = v.getIndex(); // Actual index
        int colors = v.getAvailableColors();

        int maxLoop = um ? maxColor : Math.min(maxColorCurrGraph + 1, maxColor);
        // Every coloring should be tried for um, as this is different for it.

        // We are coloring this index
        vertexIsColored |= 1 << vertexIndex;

        boolean lastToColor = (maxColoring & ~vertexIsColored) == 0;

        boolean neighboursColored = (v.getOpenNeighbourhood() & vertexIsColored) == v.getOpenNeighbourhood();

        for (int i = colors; i != 0; i &= i - 1) {
            int color = Integer.numberOfTrailingZeros(i);
            if (color > maxLoop) break; // We passed the highest possible color in the graph
            if (lastToColor && maxColorCurrGraph < maxColor && color <= maxColorCurrGraph) continue; // We don't want to retry already tried states


            v.changeColor(color + 1); // + 1 as the actual colors are from 1 to n

            // We have to now check if all our neighbours are colored, as this isn't checked in updateNeighbours
            // This is an extra check for correctness
            if (neighboursColored && !v.isCorrectlyColored(coloring,  verticesIndexed, false, false, open, proper, um)) {
                // This color isn't correct, we pick another
                continue;
            }

            // We also change the available colors for the neighbours if the coloring is proper
            ArrayList<VertexLL> changed =  new ArrayList<>();

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
                for (VertexLL neighbour : changed) {
                    neighbour.addColorFromAvailableColors(color);

                    neighbour.removeFromLL(vertices, neighbour.getAmountOfAvailableColors() - 2);
                    neighbour.addToLL(vertices, neighbour.getAmountOfAvailableColors() - 1);
                }
            }

        }

        // We decolor this vertex
        vertexIsColored &= ~(1 << vertexIndex);
        v.changeColor(0);

        // We add the vertex back to be chosen.
        v.addToLL(vertices[v.getAmountOfAvailableColors() - 1]);
        vertices[v.getAmountOfAvailableColors() - 1] = v;

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
     *          this also includes odd coloring.
     * @param   proper
     *          Whether the coloring is proper.
     * @param   um
     *          Whether the coloring is a version of unique-maximum coloring.
     * @param   changed
     *          This should be an empty list
     *          to be filled with the vertices that were changed.
     *          This will contain the old vertices that were removed from verticesIndexed.
     * @return  Whether we should skip this color
     *          as we already found a neighbour with zero possible colors.
     */
    private boolean updateNeighbours(VertexLL v, int color, Coloring coloring, boolean open, boolean proper, boolean um, ArrayList<VertexLL> changed) {
        for (int i = v.getOpenNeighbourhood(); i != 0; i &= i - 1) {
            int bit = Integer.numberOfTrailingZeros(i);
            VertexLL neighbour = verticesIndexed[bit];

            boolean neighbourIsColored = ((1 << bit) & vertexIsColored) > 0;

            if ((neighbour.getOpenNeighbourhood() & vertexIsColored) == neighbour.getOpenNeighbourhood() &&
                    neighbourIsColored) {
                // All the neighbour's neighbours are colored and the neighbour itself is colored
                // We want to check if the neighbour is CORRECTLY colored
                if (!neighbour.isCorrectlyColored(coloring, verticesIndexed, false, false, open, proper, um)) {
                    // Early pruning
                    for (VertexLL changedNeighbour : changed) {
                        // We add the colors back
                        changedNeighbour.addColorFromAvailableColors(color);

                        // We change back the neighbour LL's
                        changedNeighbour.removeFromLL(vertices, changedNeighbour.getAmountOfAvailableColors() - 2);
                        changedNeighbour.addToLL(vertices, changedNeighbour.getAmountOfAvailableColors() - 1);
                    }
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
                        neighbour.removeFromLL(vertices, amountOfAvailableColors);
                        neighbour.addToLL(vertices, amountOfAvailableColors - 1);
                    } else {
                        // Early pruning
                        for (VertexLL changedNeighbour : changed) {
                            // We add the colors back
                            changedNeighbour.addColorFromAvailableColors(color);

                            // We change back the neighbour LL's
                            changedNeighbour.removeFromLL(vertices, changedNeighbour.getAmountOfAvailableColors() - 2);
                            changedNeighbour.addToLL(vertices, changedNeighbour.getAmountOfAvailableColors() - 1);
                        }
                        // The neighbour here isn't added to changed, we do this separately
                        neighbour.addColorFromAvailableColors(color);

                        return true;
                    }

                }

            }
        }
        return false;
    }

}
