#include <gtest/gtest.h>

#include "raiigraph/initialize.hpp"

TEST(Initialize, Basic) {
    raiigraph::initialize();
    EXPECT_FALSE(raiigraph::initialize());
}
