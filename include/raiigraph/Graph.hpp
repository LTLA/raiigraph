#ifndef RAIIGRAPH_GRAPH_HPP 
#define RAIIGRAPH_GRAPH_HPP

#include "igraph.h"
#include "Vector.hpp"

/**
 * @file raiigraph.hpp
 *
 * @brief Utilities for manipulating **igraph** data structures in C++.
 */

namespace raiigraph {

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
    Graph(const IntVector& edges, igraph_integer_t num_vertices, igraph_bool_t directed) : Graph(edges.get(), num_vertices, directed) {} 

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
    Graph(igraph_t&& graph) : my_graph(std::move(graph)) {}

    /**
     * @param other Graph to be copy-constructed from.
     */
    Graph(const Graph& other) {
        if (igraph_copy(&my_graph, &(other.my_graph))) {
            throw std::runtime_error("failed to copy-construct an igraph graph object"); 
        }
    }

    /**
     * @param other Graph to be copy-assigned from.
     */
    Graph& operator=(const Graph& other) {
        if (this != &other) {
            if (igraph_copy(&my_graph, &(other.my_graph))) {
                throw std::runtime_error("failed to copy-assign an igraph graph object"); 
            }
        }
        return *this;
    }

    /**
     * @param other Graph to be move-constructed from.
     * This constructor will leave `other` in a valid but unspecified state.
     */
    Graph(Graph&& other) {
        setup(0, false);
        std::swap(my_graph, other.my_graph);
    }    

    /**
     * @param other Graph to be move-assigned from.
     * This constructor will leave `other` in a valid but unspecified state.
     */
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

public:
    /**
     * @return Pointer to the underlying **igraph** vector object.
     * This is guaranteed to be non-NULL and initialized.
     */
    operator igraph_t*() {
        return &my_graph;
    }

    /**
     * @return Const pointer to the underlying **igraph** vector object.
     * This is guaranteed to be non-NULL and initialized.
     */
    operator const igraph_t*() const {
        return &my_graph;
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
