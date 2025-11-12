public class VertexLL {

    /**
     * The color of a vertex.
     *
     * @note    The color 0 has no meaning as we use the colors {1,...,k}
     */
    private int color = 0;

    /**
     * The neighbours of a vertex, presented with bitsets.
     */
    private int neighbours = 0;

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
     * The next vertex in the linked list.
     *
     * @note    This can be a null pointer.
     */
    private VertexLL next = null;

    /**
     * The previous vertex in the linked list.
     *
     * @note    This can be a null pointer.
     */
    private VertexLL previous = null;

    /**
     * A constructor for a vertex object.
     */
    public VertexLL(int index) {
        this.index = index;
    }

    public VertexLL(VertexLL v) {
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
    public void addNeighbour(VertexLL neighbour) {
        neighbours |= (1 << neighbour.index);
        neighbour.addSingleNeighbour(this);
        incrementDegree();
    }

    /**
     * A method for adding a single neighbour. This is only used for adding bidirectional neighbours.
     *
     * @param   neighbour
     *          The neighbour to add to the neighbours number.
     */
    void addSingleNeighbour(VertexLL neighbour) {
        neighbours |= (1 << neighbour.index);
        incrementDegree();
    }

    /**
     * A method for acquiring a list of neighbours of a vertex.
     * This method returns the open neighbourhood of a vertex,
     * therefore this does not include this vertex.
     *
     * @note    This method gives the neighbour list directly,
     *          changing the given list changes the actual list of neighbours.
     */
    public int getOpenNeighbourhood() {
        return neighbours;
    }


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
     * A method for removing this vertex from the current linked list it's in.
     */
    public void removeFromLL(VertexLL[] vertices, int index) {
        if (next == null) {
            if (previous != null) {
                previous.next = null;
            } else {
                // We're the first and only element
                vertices[index] = null;
            }
        } else {
            if (previous == null) {
                // We're the first element, we should update vertices
                vertices[index] = next;
                next.previous = null;
            } else {
                previous.next = next;
                next.previous = previous;
            }
        }
        previous = null;
        next = null;
    }

    /**
     * A method for adding a vertex into a linked list representation.
     *
     * @param   startingVertex
     *          The starting vertex of the linked list this vertex should get added into.
     * @return  Whether the vertex as successfully added.
     */
    public boolean addToLL(VertexLL startingVertex) {
        // We need to remove this vertex first
        if (next != null || previous != null) return false;
        if (startingVertex != null) {
            // The vertex needs to actually be the first
            if (startingVertex.previous != null) return false;

            startingVertex.previous = this;
            next = startingVertex;
        }
        return true;
    }

    /**
     * A method for adding a vertex into a linked list representation.
     * This method also changes the starting vertex of the given array.
     *
     * @param   vertices
     *          The array of linked lists to add into.
     * @param   index
     *          The index to add this vertex to for the given array.
     * @return  Whether the vertex as successfully added.
     */
    public boolean addToLL(VertexLL[] vertices, int index) {
        VertexLL startingVertex = vertices[index];
        // We need to remove this vertex first
        if (next != null || previous != null) return false;
        if (startingVertex != null) {
            // The vertex needs to actually be the first
            if (startingVertex.previous != null) return false;

            startingVertex.previous = this;
            next = startingVertex;
        }
        vertices[index] = this;
        return true;
    }

    /**
     * A method for easily resetting a vertex.
     */
    public void resetLL() {
        next = null;
        previous = null;
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
     * @param   verticesIndexed
     *          The vertices array of the indexed vertices.
     *          These are the actual correct vertices to be used.
     * @param   properLy
     *          Whether the fact that the vertex is proper should get checked.
     */
    public boolean isCorrectlyColored(Coloring inputColoring, VertexLL[] verticesIndexed, boolean properLy) {
        boolean open = Coloring.isOpen(inputColoring);
        boolean proper = Coloring.isProper(inputColoring);
        int n = inputColoring.getMaxChromaticNumber();
        int[] colors = new int[n];
        // This is created with a length of ten, as the most upper bound of any chromatic number is 10
        // We should therefore only use this method when the inputColoring has happened.

        for (int i = 0; i <= 31 - Integer.numberOfLeadingZeros(neighbours); i++) {
            if ((neighbours & 1 << i) == 0) continue;

            VertexLL neighbour = verticesIndexed[i];

            if (neighbour.getColor() == 0) {
                return false;
                // Not all vertices have been colored yet, this vertex is also checked.
            }

            if (properLy && proper && neighbour.getColor() == color && !this.equals(neighbour)) {
                return false;
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

            for (int i = 0; i < n; i++) {
                if (colors[i] % 2 == 1) {
                    return true;
                }
            }
            return false;
        } else if (Coloring.isConflictFree(inputColoring)) {
            // Conflict-free check

            for (int i = 0; i < n; i++) {
                if (colors[i] == 1) {
                    return true;
                }
            }
            return false;
        } else {
            // Unique-maximum check

            for (int i = n-1; i >= 0; i--) {
                if (colors[i] == 0) {
                    continue;
                }
                return colors[i] == 1;
            }
        }

        return false;
    }

}
