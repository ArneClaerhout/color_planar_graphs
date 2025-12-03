import java.util.*;
import java.util.stream.Stream;

import static java.util.stream.Collectors.toList;

public class GraphPQ extends Graph {

    /**
     * The Priority Queue comprised of the vertices in this graph.
     * Sorted by the amount of available colors left.
     */
    private PriorityQueue<Vertex> vertices = new PriorityQueue<>(Comparator.comparingInt(Vertex::getAmountOfAvailableColors));

    public GraphPQ(String graph6) {
        super(graph6);
        vertices.addAll(List.of(super.verticesIndexed));
    }

    /**
     * A constructor for the graph class using the adjacency matrix of a graph.
     */
    public GraphPQ(int[][] adjMatrix) {
        super(adjMatrix);
        vertices.addAll(List.of(super.verticesIndexed));
    }

    /**
     * {@inheritDoc}
     */
    @Override
    public int findChromaticNumberOptimized(Coloring coloring, boolean open, boolean proper, boolean um, boolean checkCondition, boolean allColorings) {
        int n = coloring.getMaxChromaticNumber();

        for (int i = 2; i <= n; i++) {
            vertexIsColored = 0;
            vertices.clear();
            vertices.addAll(List.of(this.verticesIndexed));
            for (Vertex v : vertices) {
                v.setMaxAvailableColors(i);
                // Note that this changes the amount of available colors
                // This does not matter for the PQ as they're all the same value
            }
            if (optimizedAlgorithm(coloring, open, proper, um, 0, i, checkCondition, allColorings)) {
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

        for (Vertex v : verticesIndexed) {
            if ((vertexIsColored & (1 << v.getIndex())) == 0 && !vertices.contains(v)) {
                System.out.println(v.getIndex());
            }
        }

        Vertex v = vertices.poll();

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

            // We create the changed ArrayList for this vertex here, as we can use this later on
            ArrayList<Vertex> changed = new ArrayList<>();


            // We also change the available colors for the neighbours if the coloring is proper
            if (updateNeighbours(v.getOpenNeighbourhood(), color, coloring, open, proper, um, false, changed)) {
                // We enter this if statement when we find an early prune, found by updateNeighbours
                continue;
            }


            int newMaxColorCurrGraph = Math.max(maxColorCurrGraph, color + 1);
            if (optimizedAlgorithm(coloring, open, proper, um, newMaxColorCurrGraph, maxColor, checkCondition, allColorings)) {
                return true;
            }


            // We add back the available colors if it didn't work out
            if (proper) {
                updateNeighbours(v.getOpenNeighbourhood(), color, coloring, open, proper, um, true, changed);
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
     * @param   color
     *          The color that should get added or removed
     *          from the neighbours available colors array.
     * @param   coloring
     *          The coloring used.
     * @param   open
     *          Whether the coloring is an open coloring,
     *          this also includes odd coloring.µ
     * @param   proper
     *          Whether the coloring is proper.
     * @param   um
     *          Whether the coloring is a unique-maximum coloring.
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
    private boolean updateNeighbours(int neighbourhood, int color, Coloring coloring, boolean open, boolean proper, boolean um, boolean addColor, ArrayList<Vertex> changed) {
        if (addColor) {
            // We need to undo everything in changed
            for (Vertex changedNeighbour : changed) {
                // This readds the changed neighbours to the actual vertices array
                verticesIndexed[changedNeighbour.getIndex()] = changedNeighbour;
                vertices.add(changedNeighbour);
                // We also readd the changedNeighbours back as they could have been removed already.
            }
            return false; // This doesn't matter as we don't use this in this context
        }

        for (int i = neighbourhood; i != 0; i &= i - 1) {
            int bit = Integer.numberOfTrailingZeros(i);
            Vertex neighbour = verticesIndexed[bit];

            boolean neighbourIsColored = ((1 << bit) & vertexIsColored) > 0;

            if ((neighbour.getOpenNeighbourhood() & vertexIsColored) == neighbour.getOpenNeighbourhood() &&
                    neighbourIsColored) {
                // All the neighbour's neighbours are colored and the neighbour itself is colored
                // We want to check if the neighbour is CORRECTLY colored
                if (!neighbour.isCorrectlyColored(coloring, verticesIndexed, false, false, open, proper, um)) {
                    // Early pruning
                    for (Vertex changedNeighbour : changed) {
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
                Vertex newNeighbour = new Vertex(neighbour);

                // We remove the colors and check for early pruning
                if (newNeighbour.removeColorFromAvailableColors(color)) {
                    if (newNeighbour.getAmountOfAvailableColors() == 0) {
                        for (Vertex changedNeighbour : changed) {
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

}
