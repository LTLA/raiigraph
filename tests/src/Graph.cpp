#include <gtest/gtest.h>

#include "raiigraph/Graph.hpp"
#include <random>

TEST(Graph, Basic) {
    std::mt19937_64 rng(42);
    std::normal_distribution distr;
    std::uniform_real_distribution distu;

    size_t nobs = 100;
    std::vector<igraph_integer_t> edges;
    for (size_t o = 0; o < nobs; ++o) {
        for (size_t o2 = 0; o2 < o; ++o2) {
            if (distu(rng) < 0.2) {
                edges.push_back(o);
                edges.push_back(o2);
            }
        }
    }
    size_t nedges = edges.size() / 2;

    igraph_vector_int_t edge_view;
    igraph_vector_int_view(&edge_view, edges.data(), edges.size());

    // Basic checks for graph capabilities.
    raiigraph::Graph graph(&edge_view, nobs, IGRAPH_DIRECTED);
    EXPECT_EQ(graph.is_directed(), IGRAPH_DIRECTED);
    EXPECT_EQ(graph.ecount(), nedges);
    EXPECT_EQ(graph.vcount(), nobs);
    EXPECT_FALSE(graph.get() == NULL);
    EXPECT_FALSE(const_cast<const raiigraph::Graph*>(&graph)->get() == NULL);

    // Trying copy construction/assignment to get some coverage.
    {
        raiigraph::Graph g2(graph);
        EXPECT_EQ(g2.ecount(), nedges);
        EXPECT_EQ(g2.vcount(), nobs);
        EXPECT_FALSE(graph.get() == g2.get());

        raiigraph::Graph g3;
        EXPECT_EQ(g3.vcount(), 0);
        EXPECT_EQ(g3.ecount(), 0);

        g3 = g2;
        EXPECT_EQ(g3.vcount(), nobs);
        EXPECT_EQ(g3.ecount(), nedges);
        EXPECT_FALSE(g3.get() == g2.get());
    }

    // Trying move construction/assignment to get some coverage.
    {
        auto copy = graph;
        raiigraph::Graph g2(std::move(copy));
        EXPECT_EQ(g2.ecount(), nedges);
        EXPECT_EQ(g2.vcount(), nobs);

        raiigraph::Graph g3;
        g3 = std::move(g2);
        EXPECT_EQ(g3.vcount(), nobs);
        EXPECT_EQ(g3.ecount(), nedges);
    }

    // Taking ownership.
    {
        igraph_t tmp;
        igraph_empty(&tmp, 100, IGRAPH_UNDIRECTED);
        raiigraph::Graph owner(std::move(tmp));
        EXPECT_EQ(owner.vcount(), 100);
        EXPECT_EQ(owner.ecount(), 0);
        EXPECT_EQ(owner.is_directed(), IGRAPH_UNDIRECTED);
    }
}

TEST(Graph, Coercion) {
    raiigraph::Graph tmp;
    EXPECT_EQ(tmp.get(), static_cast<igraph_t*>(tmp));

    const auto& cref = tmp;
    EXPECT_EQ(cref.get(), static_cast<const igraph_t*>(cref));
}


