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
    private ArrayList<Vertex> neighbours = new ArrayList<>();

    /**
     * A constructor for a vertex object.
     */
    public Vertex() {

    }

    /**
     * A method for adding a neighbour to this vertex.
     * The method also adds this vertex to the list of neighbours of the given vertex.
     *
     * @param   neighbour
     *          The neighbour vertex to make an edge between.
     */
    public void addNeighbour(Vertex neighbour) {
        neighbours.add(neighbour);
        neighbour.getOpenNeighbourhood().add(this);
    }

    /**
     * A method for acquiring a list of neighbours of a vertex.
     * This method returns the open neighbourhood of a vertex,
     * therefore this does not include this vertex.
     *
     * @note    This method gives the neighbour list directly,
     *          changing the given list changes the actual list of neighbours.
     */
    public ArrayList<Vertex> getOpenNeighbourhood() {
        return neighbours;
    }

    /**
     * A method for acquiring a list of neighbours of a vertex.
     * This method returns the closed neighbourhood of a vertex,
     * therefore this does include this vertex.
     *
     * @note    As we are adding this vertex to the list of neighbours,
     *          we create a copy of the neighbour list first.
     */
    public ArrayList<Vertex> getClosedNeighbourhood() {
        ArrayList<Vertex> returnList = new ArrayList<>(neighbours);
        returnList.add(this);
        return returnList;
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
     * This method checks whether the vertex is correctly colored according to a certain inputColoring method.
     *
     * @param   inputColoring
     *          The inputColoring method used.
     */
    public boolean isCorrectlyColored(Coloring inputColoring) {
        boolean open = Coloring.isOpen(inputColoring);
        boolean proper = Coloring.isProper(inputColoring);
        int[] colors = new int[10];
        // This is created with a length of ten, as the most upper bound of any chromatic number is 10
        // We should therefore only use this method when the inputColoring has happened.

        for (Vertex neighbour : getClosedNeighbourhood()) {
            if (proper && neighbour.getColor() == color && !this.equals(neighbour)) {
                // Check if the inputColoring is proper
                return false;
            }
            colors[neighbour.getColor()-1]++;
        }

        // We should remove this vertex from the colors list
        if(open) {
            colors[getColor()-1]--;
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
