#include <gtest/gtest.h>

#include "raiigraph/Matrix.hpp"
#include <random>

TEST(Matrix, Construction) {
    raiigraph::IntMatrix empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.nrow(), 0);
    EXPECT_EQ(empty.ncol(), 0);
    EXPECT_EQ(empty.size(), 0);
    EXPECT_FALSE(empty.get() == NULL);
    EXPECT_FALSE(const_cast<const raiigraph::IntMatrix*>(&empty)->get() == NULL);

    raiigraph::IntMatrix ivec(50, 20);
    EXPECT_FALSE(ivec.empty());
    EXPECT_EQ(ivec.size(), 1000);
    EXPECT_EQ(ivec.nrow(), 50);
    EXPECT_EQ(ivec.ncol(), 50);
    EXPECT_FALSE(ivec.get() == NULL);

//    // Trying copy construction/assignment to get some coverage.
//    {
//        raiigraph::IntMatrix ivec2(ivec);
//        EXPECT_EQ(ivec2.size(), 50);
//        EXPECT_FALSE(ivec2.get() == ivec.get());
//
//        igraph_vector_int_set(ivec2, 0, 10);
//        EXPECT_EQ(igraph_vector_int_get(ivec2, 0), 10);
//        EXPECT_EQ(igraph_vector_int_get(ivec, 0), 0); // doesn't affect the original.
//
//        raiigraph::IntMatrix ivec3;
//        ivec3 = ivec2;
//        EXPECT_EQ(ivec3.size(), 50);
//        EXPECT_FALSE(ivec3.get() == ivec2.get());
//
//        igraph_vector_int_set(ivec3, 0, 20);
//        EXPECT_EQ(igraph_vector_int_get(ivec3, 0), 20);
//        EXPECT_EQ(igraph_vector_int_get(ivec2, 0), 10); // doesn't affect the original.
//    }
//
//    // Trying move construction/assignment to get some coverage.
//    {
//        auto copy = ivec;
//        raiigraph::IntMatrix ivec2(std::move(copy));
//        EXPECT_EQ(ivec2.size(), 50);
//        EXPECT_NE(copy.size(), 50); // moved from...
//        EXPECT_FALSE(ivec2.get() == ivec.get());
//
//        raiigraph::IntMatrix ivec3;
//        ivec3 = std::move(ivec2);
//        EXPECT_EQ(ivec3.size(), 50);
//        EXPECT_NE(ivec2.size(), 50); // moved from...
//        EXPECT_FALSE(ivec3.get() == ivec2.get());
//    }
//
//    // Taking ownership.
//    {
//        igraph_vector_int_t tmp;
//        igraph_vector_int_init(&tmp, 100);
//        raiigraph::IntMatrix owner(std::move(tmp));
//        EXPECT_EQ(owner.size(), 100);
//    }
//
//    {
//        std::vector<double> values { 0, 1, 2, 3 };
//        raiigraph::IntMatrix contents(values.begin(), values.end());
//        EXPECT_EQ(contents.size(), 4);
//        EXPECT_EQ(contents[0], 0);
//        EXPECT_EQ(contents[3], 3);
//    }
}
