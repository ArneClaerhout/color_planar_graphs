import java.util.ArrayList;

public class Vertex {

    /**
     * The color of a vertex.
     *
     * @note    The color 0 has no meaning as we use the colors {1,...,k}
     */
    private int color = 0;

    /**
     * The neighbours of a vertex.
     */
    private boolean[] neighbours;

    /**
     * The available colors for this vertex.
     */
    private boolean[] availableColors = new boolean[10];

    /**
     * The number of available colors for this vertex.
     */
    private int amountOfAvailableColors = 0;

    /**
     * The degree of this vertex.
     */
    private int degree = 0;

    /**
     * The index of this vertex.
     *
     * @note    This variable is only needed for the show option in colorScript.sh.
     *          As we use this to correctly color the shown graph.
     */
    private final int index;

    /**
     * A constructor for a vertex object.
     */
    public Vertex(int index, int numberOfVertices) {
        this.neighbours = new boolean[numberOfVertices];
        this.index = index;
    }

    public Vertex(Vertex v) {
        this.color = v.color;
        // We don't have to make a new copy of the list and the array, as they don't
        this.neighbours = v.neighbours;
        this.availableColors = v.availableColors.clone();

        this.amountOfAvailableColors = v.amountOfAvailableColors;
        this.degree = v.degree;
        this.index = v.index;
    }

    /**
     * A method for adding a neighbour to this vertex.
     * The method also adds this vertex to the list of neighbours of the given vertex.
     *
     * @param   neighbour
     *          The neighbour vertex to make an edge between.
     */
    public void addNeighbour(Vertex neighbour) {
        neighbours[neighbour.index] = true;
        neighbour.getOpenNeighbourhood()[this.index] = true;
        incrementDegree();
        neighbour.incrementDegree();
    }

    /**
     * A method for acquiring a list of neighbours of a vertex.
     * This method returns the open neighbourhood of a vertex,
     * therefore this does not include this vertex.
     *
     * @note    This method gives the neighbour list directly,
     *          changing the given list changes the actual list of neighbours.
     */
    public boolean[] getOpenNeighbourhood() {
        return neighbours;
    }

//    /**
//     * A method for acquiring a list of neighbours of a vertex.
//     * This method returns the closed neighbourhood of a vertex,
//     * therefore this does include this vertex.
//     *
//     * @note    As we are adding this vertex to the list of neighbours,
//     *          we create a copy of the neighbour list first.
//     */
//    public boolean[] getClosedNeighbourhood() {
//        return
//        ArrayList<Vertex> returnList = new ArrayList<>(neighbours);
//        returnList.add(this);
//        return returnList;
//    }


    /**
     * A method for acquiring the color of a vertex.
     */
    public int getColor() {
        return color;
    }


    /**
     * A method for changing the color of this vertex.
     *
     * @param   color
     *          The color to which this vertex is changed.
     */
    public void changeColor(int color) {
        this.color = color;
    }

    /**
     * A method for getting the amount of available colors left for this vertex.
     * This is equal to the amount of true values in the availableColors array.
     */
    public int getAmountOfAvailableColors() {
        return amountOfAvailableColors;
    }

    /**
     * A method for setting the amount of available colors for this vertex.
     */
    private void setAmountOfAvailableColors(int amountOfAvailableColors) {
        if (amountOfAvailableColors >= 0 &&  amountOfAvailableColors <= 10) {
            this.amountOfAvailableColors = amountOfAvailableColors;
        }
    }

    /**
     * A method for acquiring the available colors for this vertex.
     */
    public boolean[] getAvailableColors() {
        return availableColors;
    }

    /**
     * A method for changing the available colors to a given list.
     */
    public void setAvailableColors(boolean[] availableColors) {
        int count = 0;
        this.availableColors = availableColors;
        for  (int i = 0; i < availableColors.length; i++) {
            if (availableColors[i]) {
                count++;
            }
        }
        setAmountOfAvailableColors(count);
    }

    /**
     * A method for resetting the available colors to a list with numbers from 1 to max.
     */
    public void setMaxAvailableColors(int max) {
        for (int i = 0; i < max; i++) {
            availableColors[i] = true;
        }
        for (int i = max; i < 10; i++) {
            availableColors[i] = false;
        }
        setAmountOfAvailableColors(max);
    }

    /**
     * Removes a color from the available color array.
     */
    public boolean removeColorFromAvailableColors(int color) {
        if (availableColors[color]) {
            this.availableColors[color] = false;
            setAmountOfAvailableColors(amountOfAvailableColors - 1);
            return true;
        }
        return false;
    }

    /**
     * Adds a color to the available color array.
     */
    public boolean addColorFromAvailableColors(int color) {
        if (!availableColors[color]) {
            this.availableColors[color] = true;
            setAmountOfAvailableColors(amountOfAvailableColors + 1);
            return true;
        }
        return false;
    }

    /**
     * A method for acquiring the index of this vertex,
     * according to the adjacency list of the graph6 string given to the Graph class.
     */
    public int getIndex() {
        return index;
    }

    /**
     * A method for acquiring the degree of this vertex.
     */
    public int getDegree() {
        return degree;
    }

    /**
     * A package-private method for incrementing the degree of this vertex.
     */
    void incrementDegree() {
        this.degree++;
    }

    /**
     * This method checks whether the vertex is correctly colored according to a certain inputColoring method.
     *
     * @param   inputColoring
     *          The inputColoring method used.
     * @param   properLy
     *          Whether the fact that the vertex is proper should get checked.
     */
    public boolean isCorrectlyColored(Coloring inputColoring, Vertex[] verticesIndexed, boolean properLy) {
        boolean open = Coloring.isOpen(inputColoring);
        boolean proper = Coloring.isProper(inputColoring);
        boolean[] neighbours = getOpenNeighbourhood();
        int[] colors = new int[10];
        // This is created with a length of ten, as the most upper bound of any chromatic number is 10
        // We should therefore only use this method when the inputColoring has happened.

        for (int i = 0; i < neighbours.length; i++) {
            if (!neighbours[i]) continue;

            Vertex neighbour = verticesIndexed[i];

            if (neighbour.getColor() == 0) {
                return false;
                // Not all vertices have been colored yet, this vertex is also checked.
            }

            if (properLy) {
                if (proper && neighbour.getColor() == color && !this.equals(neighbour)) {
                    // Check if the inputColoring is proper
                    return false;
                }
            }

            colors[neighbour.getColor()-1]++;
        }

        // We should add this vertex to the colors list, if closed coloring
        if(!open) {
            colors[getColor()-1]++;
        }

        if (inputColoring == Coloring.PROPER) {
            // Proper check

            return true;
        } else if (inputColoring == Coloring.ODD) {
            // Odd check

            for (int i = 0; i < 10; i++) {
                if (colors[i] % 2 == 1) {
                    return true;
                }
            }
            return false;
        } else if (Coloring.isConflictFree(inputColoring)) {
            // Conflict-free check

            for (int i = 0; i < 10; i++) {
                if (colors[i] == 1) {
                    return true;
                }
            }
            return false;
        } else {
            // Unique-maximum check

            for (int i = 9; i >= 0; i--) {
                if (colors[i] == 0) {
                    continue;
                }
                if (colors[i] == 1) {
                    return true;
                    // If the first color we find (the max color) is 1, return true
                }
                return false;
            }
        }

        return false;
    }

}
