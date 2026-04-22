package graphs;

public class Vertex {

    /**
     * The color of a vertex.
     *
     * @note    The color 0 has no meaning as we use the colors {1,...,k}
     */
    private int color = 0;

    /**
     * The neighbours of a vertex, presented with bitsets.
     */
    private long neighbours = 0;

    /**
     * The available colors for this vertex.
     */
    private int availableColors = 0;

    /**
     * The number of available colors for this vertex.
     */
    private int amountOfAvailableColors = 0;

    /**
     * The degree of this vertex.
     */
    private int degree = 0;

    private int amountOfColoredNeighbours = 0;

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
    public Vertex(int index) {
        this.index = index;
    }

    public Vertex(Vertex v) {
        this.color = v.color;
        // We don't have to make a new copy of the list and the array, as they don't
        this.neighbours = v.neighbours;
        this.availableColors = v.availableColors;

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
        neighbours |= (1L << neighbour.index);
        neighbour.addSingleNeighbour(this);
        incrementDegree();
    }

    /**
     * A method for adding a single neighbour. This is only used for adding bidirectional neighbours.
     *
     * @param   neighbour
     *          The neighbour to add to the neighbours number.
     */
    protected void addSingleNeighbour(Vertex neighbour) {
        neighbours |= (1L << neighbour.index);
        incrementDegree();
    }

    /**
     * A method for removing a neighbour, this is done for both parties.
     */
    protected void removeNeighbour(Vertex neighbour) {
        neighbours &= ~(1L << neighbour.index);
        neighbour.neighbours &= ~(1L << this.index);
        decrementDegree();
        neighbour.decrementDegree();
    }

    /**
     * A method for acquiring a list of neighbours of a vertex.
     * This method returns the open neighbourhood of a vertex,
     * therefore this does not include this vertex.
     *
     * @note    This method gives the neighbour list directly,
     *          changing the given list changes the actual list of neighbours.
     */
    public long getOpenNeighbourhood() {
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
    public int getAvailableColors() {
        return availableColors;
    }

    /**
     * A method for changing the available colors to a given list.
     */
    public void setAvailableColors(int availableColors) {
        int count = 0;
        this.availableColors = availableColors;
        for  (int i = availableColors; i != 0; i &= i - 1) {
            count++;
        }
        setAmountOfAvailableColors(count);
    }

    /**
     * A method for resetting the available colors to a list with numbers from 1 to max.
     */
    public void setMaxAvailableColors(int max) {
        availableColors = (1 << max) - 1;
        setAmountOfAvailableColors(max);
    }

    /**
     * Removes a color from the available color array.
     */
    public boolean removeColorFromAvailableColors(int color) {
        if ((availableColors & (1 << color)) != 0) {
            this.availableColors &= ~(1 << color);
            amountOfAvailableColors--;
            return true;
        }
        return false;
    }

    /**
     * Adds a color to the available color array.
     */
    public boolean addColorFromAvailableColors(int color) {
        if ((availableColors & (1 << color)) == 0) {
            this.availableColors |= 1 << color;
            amountOfAvailableColors++;
            return true;
        }
        return false;
    }

    /**
     * A method for acquiring the index of this vertex,
     * according to the adjacency list of the graph6 string given to the Graphs.Graph class.
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

    void decrementDegree() {
        this.degree--;
    }

    public int getImportanceValue() {
        return getAmountOfAvailableColors() + (getDegree() - amountOfColoredNeighbours);
    }

    public int getAmountOfColoredNeighbours() {
        return amountOfColoredNeighbours;
    }

    public void setAmountOfColoredNeighbours(int amountOfColoredNeighbours) {
        this.amountOfColoredNeighbours = amountOfColoredNeighbours;
    }

    public void incrementAmountOfColoredNeighbours() {
        this.amountOfColoredNeighbours++;
    }

    public void decrementAmountOfColoredNeighbours() {
        this.amountOfColoredNeighbours--;
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
     * @param   fillUncolored
     *          Whether vertices that aren't colored but have only one possible color
     *          are allowed. The one possible color will be used as the color.
     * @param   open
     *          Whether the coloring is an open coloring,
     *          this also includes odd coloring.µ
     * @param   proper
     *          Whether the coloring is proper.
     * @param   um
     *          Whether the coloring is a unique-maximum coloring.
     */
    public boolean isCorrectlyColored(Coloring inputColoring, Vertex[] verticesIndexed, boolean properLy,
                                      boolean fillUncolored, boolean open, boolean proper, boolean um) {
        if (!fillUncolored && color == 0 && !open) return false; // This vertex isn't colored.

        long neighbourhood = open ? neighbours : (neighbours | (1L << index));

        if (inputColoring == Coloring.ODD) {
            // Odd coloring
            int odds = 0;
            for (long i = neighbourhood; i != 0; i &= i - 1) {
                int index = Long.numberOfTrailingZeros(i);

                Vertex neighbour = verticesIndexed[index];
                int neighbourColor = neighbour.getColor();

                if (!fillUncolored && neighbourColor == 0) {
                    return false;
                    // Not all vertices have been colored yet.
                } else if (neighbourColor == 0) {
                    // fillUncolored is true, we check if we can fill the uncolored vertex.
                    if (neighbour.getAmountOfAvailableColors() == 1) {
                        odds ^= (1 << Integer.numberOfTrailingZeros(neighbour.getAvailableColors()));
                    } else {
                        return false;
                    }
                } else {
                    odds ^= (1 << neighbourColor);
                }
                if (properLy && proper && neighbourColor == color && !this.equals(neighbour)) {
                    return false;
                }
            }
            return odds != 0;
        } else if (inputColoring == Coloring.PROPER) {
            // Proper coloring
            if (!properLy) return true;
            for (long i = neighbourhood; i != 0; i &= i - 1) {
                int index = Long.numberOfTrailingZeros(i);

                Vertex neighbour = verticesIndexed[index];
                int neighbourColor = neighbour.getColor();

                if (!fillUncolored && neighbourColor == 0) {
                    return false;
                    // Not all vertices have been colored yet.
                } else if (neighbourColor == 0 && neighbour.getAmountOfAvailableColors() != 1) {
                    // fillUncolored is true, we check if we can fill the uncolored vertex.
                    return false;
                } else if (neighbourColor == 0) {
                    neighbourColor = Integer.numberOfTrailingZeros(neighbour.getAvailableColors());
                }

                if (neighbourColor == color) {
                    return false;
                }

            }
        } else if (um) {
            // Unique-Maximum
            int max = 0;
            int amountOfMax = 0;
            for (long i = neighbourhood; i != 0; i &= i - 1) {
                int index = Long.numberOfTrailingZeros(i);

                Vertex neighbour = verticesIndexed[index];
                int neighbourColor = neighbour.getColor();

                if (!fillUncolored && neighbourColor == 0) {
                    return false;
                    // Not all vertices have been colored yet.
                } else if (neighbourColor == 0) {
                    // fillUncolored is true, we check if we can fill the uncolored vertex.
                    if (neighbour.getAmountOfAvailableColors() == 1) {
                        neighbourColor = Integer.numberOfTrailingZeros(neighbour.getAvailableColors());
                    } else {
                        return false;
                    }
                }

                if (properLy && proper && neighbourColor == color && !this.equals(neighbour)) {
                    return false;
                }

                if (neighbourColor > max) {
                    max = neighbourColor;
                    amountOfMax = 1;
                } else if (neighbourColor == max) {
                    amountOfMax++;
                }
            }
            return amountOfMax == 1;
        } else {
            int colorsOccurOnce = 0;
            int colorsOccur = 0;
            int colorIndex;

            for (long i = neighbourhood; i != 0; i &= i - 1) {
                int index = Long.numberOfTrailingZeros(i);

                Vertex neighbour = verticesIndexed[index];
                int neighbourColor = neighbour.getColor();
                // We do -1 as the colors are from 1...k,
                // but we want to later on use the colors 0...k-1

                if (!fillUncolored && neighbourColor == 0) {
                    return false;
                    // Not all vertices have been colored yet.
                } else if (neighbourColor == 0) {
                    // fillUncolored is true, we check if we can fill the uncolored vertex.
                    if (neighbour.getAmountOfAvailableColors() == 1) {
                        colorIndex = 1 << Integer.numberOfTrailingZeros(neighbour.getAvailableColors());
                    } else {
                        return false;
                    }
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
            return colorsOccurOnce != 0;
        }
        return false;
    }

}
