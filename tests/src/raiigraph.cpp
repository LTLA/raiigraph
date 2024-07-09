#include <gtest/gtest.h>

#include "raiigraph/raiigraph.hpp"
#include <random>

TEST(Raiigraph, IntegerVector) {
    raiigraph::IntegerVector empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.size(), 0);
    EXPECT_FALSE(empty.get() == NULL);
    EXPECT_FALSE(const_cast<const raiigraph::IntegerVector*>(&empty)->get() == NULL);

    raiigraph::IntegerVector ivec(50);
    EXPECT_FALSE(ivec.empty());
    EXPECT_EQ(ivec.size(), 50);
    EXPECT_FALSE(ivec.get() == NULL);

    // Trying copy construction/assignment to get some coverage.
    {
        raiigraph::IntegerVector ivec2(ivec);
        EXPECT_EQ(ivec2.size(), 50);
        EXPECT_FALSE(ivec2.get() == ivec.get());

        igraph_vector_int_set(ivec2.get(), 0, 10);
        EXPECT_EQ(igraph_vector_int_get(ivec2.get(), 0), 10);
        EXPECT_EQ(igraph_vector_int_get(ivec.get(), 0), 0); // doesn't affect the original.

        raiigraph::IntegerVector ivec3;
        ivec3 = ivec2;
        EXPECT_EQ(ivec3.size(), 50);
        EXPECT_FALSE(ivec3.get() == ivec2.get());

        igraph_vector_int_set(ivec3.get(), 0, 20);
        EXPECT_EQ(igraph_vector_int_get(ivec3.get(), 0), 20);
        EXPECT_EQ(igraph_vector_int_get(ivec2.get(), 0), 10); // doesn't affect the original.
    }

    // Trying move construction/assignment to get some coverage.
    {
        auto copy = ivec;
        raiigraph::IntegerVector ivec2(std::move(copy));
        EXPECT_EQ(ivec2.size(), 50);
        EXPECT_NE(copy.size(), 50); // moved from...
        EXPECT_FALSE(ivec2.get() == ivec.get());

        raiigraph::IntegerVector ivec3;
        ivec3 = std::move(ivec2);
        EXPECT_EQ(ivec3.size(), 50);
        EXPECT_NE(ivec2.size(), 50); // moved from...
        EXPECT_FALSE(ivec3.get() == ivec2.get());
    }

    // Taking ownership.
    {
        igraph_vector_int_t tmp;
        igraph_vector_int_init(&tmp, 100);
        raiigraph::IntegerVector owner(std::move(tmp));
        EXPECT_EQ(owner.size(), 100);
    }
}

TEST(Raiigraph, RealVector) {
    raiigraph::RealVector empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.size(), 0);
    EXPECT_FALSE(empty.get() == NULL);
    EXPECT_FALSE(const_cast<const raiigraph::RealVector*>(&empty)->get() == NULL);

    raiigraph::RealVector ivec(50);
    EXPECT_FALSE(ivec.empty());
    EXPECT_EQ(ivec.size(), 50);
    EXPECT_FALSE(ivec.get() == NULL);

    // Trying copy construction/assignment to get some coverage.
    {
        raiigraph::RealVector ivec2(ivec);
        EXPECT_EQ(ivec2.size(), 50);
        EXPECT_FALSE(ivec2.get() == ivec.get());

        igraph_vector_set(ivec2.get(), 0, 10);
        EXPECT_EQ(igraph_vector_get(ivec2.get(), 0), 10);
        EXPECT_EQ(igraph_vector_get(ivec.get(), 0), 0); // doesn't affect the original.

        raiigraph::RealVector ivec3;
        ivec3 = ivec2;
        EXPECT_EQ(ivec3.size(), 50);
        EXPECT_FALSE(ivec3.get() == ivec2.get());

        igraph_vector_set(ivec3.get(), 0, 20);
        EXPECT_EQ(igraph_vector_get(ivec3.get(), 0), 20);
        EXPECT_EQ(igraph_vector_get(ivec2.get(), 0), 10); // doesn't affect the original.
    }

    // Trying move construction/assignment to get some coverage.
    {
        auto copy = ivec;
        raiigraph::RealVector ivec2(std::move(copy));
        EXPECT_EQ(ivec2.size(), 50);
        EXPECT_NE(copy.size(), 50); // moved from...
        EXPECT_FALSE(ivec2.get() == ivec.get());

        raiigraph::RealVector ivec3;
        ivec3 = std::move(ivec2);
        EXPECT_EQ(ivec3.size(), 50);
        EXPECT_NE(ivec2.size(), 50); // moved from...
        EXPECT_FALSE(ivec3.get() == ivec2.get());
    }

    // Taking ownership.
    {
        igraph_vector_t tmp;
        igraph_vector_init(&tmp, 100);
        raiigraph::RealVector owner(std::move(tmp));
        EXPECT_EQ(owner.size(), 100);
    }
}

TEST(Raiigraph, IntegerMatrix) {
    raiigraph::IntegerMatrix empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.size(), 0);
    EXPECT_EQ(empty.nrow(), 0);
    EXPECT_EQ(empty.ncol(), 0);
    EXPECT_FALSE(empty.get() == NULL);
    EXPECT_FALSE(const_cast<const raiigraph::IntegerMatrix*>(&empty)->get() == NULL);

    raiigraph::IntegerMatrix imat(50, 20);
    EXPECT_FALSE(imat.empty());
    EXPECT_EQ(imat.size(), 1000);
    EXPECT_EQ(imat.nrow(), 50);
    EXPECT_EQ(imat.ncol(), 20);
    EXPECT_FALSE(imat.get() == NULL);

    // Trying copy construction/assignment to get some coverage.
    {
        raiigraph::IntegerMatrix imat2(imat);
        EXPECT_EQ(imat2.size(), 1000);
        EXPECT_FALSE(imat2.get() == imat.get());

        igraph_matrix_int_set(imat2.get(), 0, 0, 10);
        EXPECT_EQ(igraph_matrix_int_get(imat2.get(), 0, 0), 10);
        EXPECT_EQ(igraph_matrix_int_get(imat.get(), 0, 0), 0); // doesn't affect the original.

        raiigraph::IntegerMatrix imat3;
        imat3 = imat2;
        EXPECT_EQ(imat3.size(), 1000);
        EXPECT_FALSE(imat3.get() == imat2.get());

        igraph_matrix_int_set(imat3.get(), 0, 0, 20);
        EXPECT_EQ(igraph_matrix_int_get(imat3.get(), 0, 0), 20);
        EXPECT_EQ(igraph_matrix_int_get(imat2.get(), 0, 0), 10); // doesn't affect the original.
    }

    // Trying move construction/assignment to get some coverage.
    {
        auto copy = imat;
        raiigraph::IntegerMatrix imat2(std::move(copy));
        EXPECT_EQ(imat2.size(), 1000);
        EXPECT_NE(copy.size(), 1000); // moved from...
        EXPECT_FALSE(imat2.get() == imat.get());

        raiigraph::IntegerMatrix imat3;
        imat3 = std::move(imat2);
        EXPECT_EQ(imat3.size(), 1000);
        EXPECT_NE(imat2.size(), 1000); // moved from...
        EXPECT_FALSE(imat3.get() == imat2.get());
    }

    // Taking ownership.
    {
        igraph_matrix_int_t tmp;
        igraph_matrix_int_init(&tmp, 5, 20);
        raiigraph::IntegerMatrix owner(std::move(tmp));
        EXPECT_EQ(owner.size(), 100);
    }
}

TEST(Raiigraph, Graph) {
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
