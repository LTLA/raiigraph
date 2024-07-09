#ifndef IGRAPHPP_IGRAPHPP_HPP 
#define IGRAPHPP_IGRAPHPP_HPP

#include "igraph.h"

/**
 * @file raiigraph.hpp
 *
 * @brief Utilities for manipulating **igraph** data structures in C++.
 */

/**
 * @namespace raiigraph
 * @brief Utilities for manipulating **igraph** data structures in C++.
 */
namespace raiigraph {

/**
 * @brief Control the **igraph** RNG via RAII.
 *
 * When an instance of this class is created, it will replace the global default **igraph** RNG with its own.
 * When it is destroyed, it will restore the default to the RNG that was present before its construction.
 */
class RNGScope {
public:
    /**
     * @param seed Seed for the **igraph** RNG.
     */
    RNGScope(int seed) {
        if (igraph_rng_init(&my_rng, &igraph_rngtype_mt19937)) {
            throw std::runtime_error("failed to initialize an instance of igraph's RNG");
        }

        if (igraph_rng_seed(&my_rng, seed)) {
            igraph_rng_destroy(&my_rng);
            throw std::runtime_error("failed to set the seed on igraph's RNG");
        }

        my_previous = igraph_rng_default();
        igraph_rng_set_default(&my_rng); // older versions of igraph just return void, so don't use this to set my_previous.
    }

    /**
     * @cond
     */
    // Just deleting the methods here, we shouldn't have to copy.
    RNGScope(const RNGScope& other) = delete;
    RNGScope& operator=(const RNGScope& other) = delete;
    RNGScope(RNGScope&& other) = delete;
    RNGScope& operator=(RNGScope&& other) = delete;

    ~RNGScope() {
        igraph_rng_set_default(my_previous);
        igraph_rng_destroy(&my_rng);
    }
    /**
     * @endcond
     */

private:
    igraph_rng_t* my_previous;
    igraph_rng_t my_rng;
};

/**
 * @brief Wrapper around an `igraph_vector_int_t` with RAII semantics.
 */
class IntegerVector {
private:
    void setup(igraph_integer_t size) {
        if (igraph_vector_int_init(&my_vector, size)) {
            throw std::runtime_error("failed to initialize integer igraph vector of size " + std::to_string(size));
        }
    }

public:
    /**
     * @param size Size of the vector to create.
     */
    IntegerVector(igraph_integer_t size = 0) {
        setup(size);
    }

    /**
     * @param vector An initialized vector to take ownership of.
     */
    IntegerVector(igraph_vector_int_t vector) : my_vector(std::move(vector)) {}

    /**
     * @cond
     */
    IntegerVector(const IntegerVector& other) {
        if (igraph_vector_int_init_copy(&my_vector, &(other.my_vector))) {
            throw std::runtime_error("failed to copy-construct integer igraph vector");
        }
    }

    IntegerVector& operator=(const IntegerVector& other) {
        if (this != &other) {
            if (igraph_vector_int_update(&my_vector, &(other.my_vector))) { // my_vector should already be initialized before the assignment.
                throw std::runtime_error("failed to copy-assign integer igraph vector");
            }
        }
        return *this;
    }

    IntegerVector(IntegerVector&& other) {
        setup(0); // we must leave 'other' in a valid state.
        std::swap(my_vector, other.my_vector);
    }

    IntegerVector& operator=(IntegerVector&& other) {
        if (this != &other) {
            std::swap(my_vector, other.my_vector); // 'my_vector' should already be initialized, so we're leaving 'other' in a valid state.
        }
        return *this;
    }

    ~IntegerVector() {
        igraph_vector_int_destroy(&my_vector);
    }
    /**
     * @endcond
     */

public:
    /**
     * @return Whether the vector is empty.
     */
    igraph_bool_t empty() const {
        return igraph_vector_int_empty(&my_vector);
    }

    /**
     * @return Size of the vector.
     */
    igraph_integer_t size() const {
        return igraph_vector_int_size(&my_vector);
    }

    /**
     * @return Pointer to the underlying **igraph** vector object.
     * This is guaranteed to be non-NULL and initialized.
     */
    igraph_vector_int_t* get() {
        return &my_vector;
    }

    /**
     * @return Const pointer to the underlying **igraph** vector object.
     * This is guaranteed to be non-NULL and initialized.
     */
    const igraph_vector_int_t* get() const {
        return &my_vector;
    }

private:
    igraph_vector_int_t my_vector;
};

/**
 * @brief Wrapper around an `igraph_vector_t` with RAII semantics.
 */
class RealVector {
private:
    void setup(igraph_integer_t size)  {
        if (igraph_vector_init(&my_vector, size)) {
            throw std::runtime_error("failed to initialize real igraph vector of size " + std::to_string(size));
        }
    }

public:
    /**
     * @param size Size of the vector to create.
     */
    RealVector(igraph_integer_t size = 0) {
        setup(size);
    }

    /**
     * @param vector An initialized vector to take ownership of.
     */
    RealVector(igraph_vector_t vector) : my_vector(std::move(vector)) {}

    /**
     * @cond
     */
    RealVector(const RealVector& other) {
        if (igraph_vector_init_copy(&my_vector, &(other.my_vector))) {
            throw std::runtime_error("failed to copy-construct real igraph vector");
        }
    }

    RealVector& operator=(const RealVector& other) {
        if (this != &other) {
            if (igraph_vector_update(&my_vector, &(other.my_vector))) { // my_vector should already be initialized before the assignment.
                throw std::runtime_error("failed to copy-assign real igraph vector");
            }
        }
        return *this;
    }

    RealVector(RealVector&& other) {
        setup(0); // we must leave 'other' in a valid state.
        std::swap(my_vector, other.my_vector);
    }

    RealVector& operator=(RealVector&& other) {
        if (this != &other) {
            std::swap(my_vector, other.my_vector); // 'my_vector' should already be initialized, so we're leaving 'other' in a valid state.
        }
        return *this;
    }

    ~RealVector() {
        igraph_vector_destroy(&my_vector);
    }
    /**
     * @endcond
     */

public:
    /**
     * @return Whether the vector is empty.
     */
    igraph_bool_t empty() const {
        return igraph_vector_empty(&my_vector);
    }

    /**
     * @return Size of the vector.
     */
    igraph_integer_t size() const {
        return igraph_vector_size(&my_vector);
    }

    /**
     * @return Pointer to the **igraph** vector object.
     * This is guaranteed to be non-NULL and initialized.
     */
    igraph_vector_t* get() {
        return &my_vector;
    }

    /**
     * @return Const pointer to the underlying **igraph** vector object.
     * This is guaranteed to be non-NULL and initialized.
     */
    const igraph_vector_t* get() const {
        return &my_vector;
    }

private:
    igraph_vector_t my_vector;
};

/**
 * @brief Wrapper around an `igraph_matrix_int_t` with RAII semantics.
 */
class IntegerMatrix {
private:
    void setup(igraph_integer_t nrow, igraph_integer_t ncol) {
        if (igraph_matrix_int_init(&my_matrix, nrow, ncol)) {
            throw std::runtime_error("failed to initialize an integer igraph " + std::to_string(nrow) + "x" + std::to_string(ncol) + " matrix");
        }
    }

public:
    /**
     * @param nrow Number of rows in the matrix.
     * @param ncol Number of columns in the matrix.
     */
    IntegerMatrix(igraph_integer_t nrow = 0, igraph_integer_t ncol = 0) {
        setup(nrow, ncol);
    }

    /**
     * @param matrix An initialized matrix to take ownership of.
     */
    IntegerMatrix(igraph_matrix_int_t matrix) : my_matrix(std::move(matrix)) {}

    /**
     * @cond
     */
    IntegerMatrix(const IntegerMatrix& other) {
        if (igraph_matrix_int_init_copy(&my_matrix, &(other.my_matrix))) {
            throw std::runtime_error("failed to copy-construct integer igraph matrix");
        }
    }

    IntegerMatrix& operator=(const IntegerMatrix& other) {
        if (this != &other) {
            if (igraph_matrix_int_update(&my_matrix, &(other.my_matrix))) { // my_matrix should already be initialized before the assignment.
                throw std::runtime_error("failed to copy-assign integer igraph matrix");
            }
        }
        return *this;
    }

    IntegerMatrix(IntegerMatrix&& other) {
        setup(0, 0); // must leave 'other' in a valid state.
        std::swap(my_matrix, other.my_matrix);
    }

    IntegerMatrix& operator=(IntegerMatrix&& other) {
        if (this != &other) {
            std::swap(my_matrix, other.my_matrix); // 'my_matrix' should already be initialized, so we're leaving 'other' in a valid state.
        }
        return *this;
    }

    ~IntegerMatrix() {
        igraph_matrix_int_destroy(&my_matrix);
    }
    /**
     * @endcond
     */

public:
    /**
     * @return Whether the matrix is empty.
     */
    igraph_bool_t empty() const {
        return igraph_matrix_int_empty(&my_matrix);
    }

    /**
     * @return Size of the matrix, in terms of the number of elements.
     */
    igraph_integer_t size() const {
        return igraph_matrix_int_size(&my_matrix);
    }

    /**
     * @return Number of rows in the matrix.
     */
    igraph_integer_t nrow() const {
        return igraph_matrix_int_nrow(&my_matrix);
    }

    /**
     * @return Number of columns in the matrix.
     */
    igraph_integer_t ncol() const {
        return igraph_matrix_int_ncol(&my_matrix);
    }

    /**
     * @return Pointer to the underlying **igraph** matrix object.
     * This is guaranteed to be non-NULL and initialized.
     */
    igraph_matrix_int_t* get() {
        return &my_matrix;
    }

    /**
     * @return Const pointer to the underlying **igraph** matrix object.
     * This is guaranteed to be non-NULL and initialized.
     */
    const igraph_matrix_int_t* get() const {
        return &my_matrix;
    }

private:
    igraph_matrix_int_t my_matrix;
};

/**
 * @brief Wrapper around the `igraph_t` class from **igraph**.
 */
class Graph {
private:
    void setup(igraph_integer_t num_vertices, igraph_bool_t directed) {
        if (igraph_empty(&my_graph, num_vertices, directed)) {
            throw std::runtime_error("failed to initialize an empty igraph graph object"); 
        } 
    }

public:
    /**
     * Create an empty graph.
     *
     * @param num_vertices Number of vertices.
     * @param directed Whether the graph is directed.
     */
    Graph(igraph_integer_t num_vertices = 0, igraph_bool_t directed = false) {
        setup(num_vertices, directed);
    }

    /**
     * @param edges Edges between vertices, stored as a vector of non-negative vertex indices of length equal to twice the number of edges.
     * The `i`-th edge is defined from the first vertex at `edges[2 * i]` to the second vertex at `edges[2 * i + 1]`.
     * @param num_vertices Number of vertices in the graph.
     * This should be greater than the largest index in `edges`.
     * @param directed Whether the graph is directed.
     */
    Graph(const IntegerVector& edges, igraph_integer_t num_vertices, igraph_bool_t directed) : Graph(edges.get(), num_vertices, directed) {} 

    /**
     * @param edges Edges between vertices, stored as a vector of non-negative vertex indices of length equal to twice the number of edges.
     * The `i`-th edge is defined from the first vertex at `edges[2 * i]` to the second vertex at `edges[2 * i + 1]`.
     * @param num_vertices Number of vertices in the graph.
     * This should be greater than the largest index in `edges`.
     * @param directed Whether the graph is directed.
     */
    Graph(const igraph_vector_int_t* edges, igraph_integer_t num_vertices, igraph_bool_t directed) { 
        if (igraph_create(&my_graph, edges, num_vertices, directed)) {
            throw std::runtime_error("failed to initialize an igraph graph object"); 
        }
    }

    /**
     * @param graph An initialized graph to take ownership of.
     */
    Graph(igraph_t graph) : my_graph(std::move(graph)) {}

    /**
     * @cond
     */
    Graph(const Graph& other) {
        if (igraph_copy(&my_graph, &(other.my_graph))) {
            throw std::runtime_error("failed to copy-construct an igraph graph object"); 
        }
    }

    Graph& operator=(const Graph& other) {
        if (this != &other) {
            if (igraph_copy(&my_graph, &(other.my_graph))) {
                throw std::runtime_error("failed to copy-assign an igraph graph object"); 
            }
        }
        return *this;
    }

    Graph(Graph&& other) {
        setup(0, false);
        std::swap(my_graph, other.my_graph);
    }    

    Graph& operator=(Graph&& other) {
        if (this != &other) {
            std::swap(my_graph, other.my_graph);
        }
        return *this;
    }

    ~Graph() {
        igraph_destroy(&my_graph);
    }
    /**
     * @endcond
     */

public:
    /**
     * @return Number of vertices in the graph.
     */
    igraph_integer_t vcount() const {
        return igraph_vcount(&my_graph);
    }

    /**
     * @return Number of edges in the graph.
     */
    igraph_integer_t ecount() const {
        return igraph_ecount(&my_graph);
    }

    /**
     * @return Whether the graph is directed.
     */
    igraph_bool_t is_directed() const {
        return igraph_is_directed(&my_graph);
    }

    /**
     * @return Pointer to the underlying **igraph** graph.
     * This is guaranteed to be non-NULL and initialized.
     */
    igraph_t* get() {
        return &my_graph;
    }

    /**
     * @return Pointer to the underlying **igraph** graph.
     * This is guaranteed to be non-NULL and initialized.
     */
    const igraph_t* get() const {
        return &my_graph;
    }

private:
    igraph_t my_graph;
};

}

#endif
