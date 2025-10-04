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
     * A method for adding a neighbour to this
     * @param neigbour
     */
    public void addNeighbour(Vertex neigbour) {
        neighbours.add(neigbour);
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
     * A method for checking whether a vertex is proper.
     */
    public boolean isProper() {
        for (Vertex vertex : neighbours) {
            if (vertex.getColor() == color) {
                return false;
            }
        }
        return true;
    }


    /**
     * A method for checking whether a vertex is odd.
     */
    public boolean isOdd() {
        int[] colors = new int[10];
        // This is created with a length of ten, as the most upper bound of any chromatic number is 10
        // We should therefore only use this method when the coloring has happened.

        for (Vertex vertex : neighbours) {
            if (vertex.getColor() == color) {
                return false;
            }
            colors[vertex.getColor()-1]++;
        }

        // We check whether the colors array contains an odd number.
        for (int i = 0; i < 10; i++) {
            if (colors[i] % 2 == 1) {
                return true;
            }
        }

        return false;
    }

    public boolean isConflictFree(boolean proper, boolean open) {

    }
}
