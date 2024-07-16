#include <gtest/gtest.h>

#include "raiigraph/RNGScope.hpp"

TEST(RNGScope, Basic) {
    int first, second;
    {
        raiigraph::RNGScope scope(10);
        first = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
        second = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
    }

    // Resets the previous RNG correctly.
    {
        raiigraph::RNGScope scope(10);
        int first2 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
        EXPECT_EQ(first, first2);

        {
            raiigraph::RNGScope scope(20);
            int first3 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
            EXPECT_NE(first, first3);
            int second3 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
            EXPECT_NE(second, second3);
        }

        int second2 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
        EXPECT_EQ(second, second2);
    }

    // Resets the previous RNG correctly with multiple constructions.
    {
        raiigraph::RNGScope scope(10);
        {
            raiigraph::RNGScope scope(20);
            raiigraph::RNGScope scope2(30);
            int first3 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
            EXPECT_NE(first, first3);
            int second3 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
            EXPECT_NE(second, second3);
        }

        int first2 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
        EXPECT_EQ(first, first2);
        int second2 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
        EXPECT_EQ(second, second2);
    }


    // Works with other RNG types.
    {
        raiigraph::RNGScope scope(&igraph_rngtype_mt19937);
        int first2 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
        int second2 = igraph_rng_get_integer(igraph_rng_default(), 0, 10000000);
        EXPECT_FALSE(first == first2 && second == second2);
    }
}
