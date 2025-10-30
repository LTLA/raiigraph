#include <gtest/gtest.h>
#include "raiigraph/raiigraph.hpp"

TEST(Initialize, Basic) {
    // Check that the static variable correctly updates.
    raiigraph::initialize();
    EXPECT_TRUE(raiigraph::initialize());
}
