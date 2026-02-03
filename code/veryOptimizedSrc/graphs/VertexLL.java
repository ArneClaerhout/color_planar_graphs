package Graphs;

public class VertexLL extends Vertex {

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
        super(index);
    }

    public VertexLL(Vertex v) {
        super(v);
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

}
