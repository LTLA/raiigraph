#include <gtest/gtest.h>

#include "raiigraph/Vector.hpp"
#include "raiigraph/initialize.hpp"

#include <random>

TEST(Vector, Construction) {
    raiigraph::initialize();

    raiigraph::IntegerVector empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.size(), 0);
    EXPECT_FALSE(empty.get() == NULL);
    EXPECT_FALSE(const_cast<const raiigraph::IntegerVector*>(&empty)->get() == NULL);

    raiigraph::IntegerVector ivec(50);
    EXPECT_FALSE(ivec.empty());
    EXPECT_EQ(ivec.size(), 50);
    EXPECT_FALSE(ivec.get() == NULL);
    for (auto x : ivec) { // check it was indeed zero-initialized.
        EXPECT_EQ(x, 0);
    }

    // Check that the non-default fill works.
    {
        raiigraph::IntVector ivec10(50, 10);
        for (auto x : ivec10) {
            EXPECT_EQ(x, 10);
        }
    }

    // Trying copy construction/assignment to get some coverage.
    {
        raiigraph::IntegerVector ivec2(ivec);
        EXPECT_EQ(ivec2.size(), 50);
        EXPECT_FALSE(ivec2.get() == ivec.get());

        igraph_vector_int_set(ivec2, 0, 10);
        EXPECT_EQ(igraph_vector_int_get(ivec2, 0), 10);
        EXPECT_EQ(igraph_vector_int_get(ivec, 0), 0); // doesn't affect the original.

        raiigraph::IntegerVector ivec3;
        ivec3 = ivec2;
        EXPECT_EQ(ivec3.size(), 50);
        EXPECT_FALSE(ivec3.get() == ivec2.get());

        igraph_vector_int_set(ivec3, 0, 20);
        EXPECT_EQ(igraph_vector_int_get(ivec3, 0), 20);
        EXPECT_EQ(igraph_vector_int_get(ivec2, 0), 10); // doesn't affect the original.
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

    {
        std::vector<double> values { 0, 1, 2, 3 };
        raiigraph::IntegerVector contents(values.begin(), values.end());
        EXPECT_EQ(contents.size(), 4);
        EXPECT_EQ(contents[0], 0);
        EXPECT_EQ(contents[3], 3);
    }
}

TEST(Vector, Access) {
    raiigraph::initialize();

    std::vector<int> foo{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    raiigraph::IntegerVector contents(foo.begin(), foo.end());
    const auto& cref = contents;

    auto it = contents.begin();
    auto cit = contents.cbegin();
    auto cit2 = cref.begin();
    auto dptr = contents.data();
    auto cdptr = cref.data();
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(*it, i);
        EXPECT_EQ(*cit, i);
        EXPECT_EQ(*cit2, i);
        EXPECT_EQ(*dptr, i);
        EXPECT_EQ(*cdptr, i);
        EXPECT_EQ(contents[i], i);
        EXPECT_EQ(cref[i], i);
        ++it;
        ++cit;
        ++cit2;
        ++dptr;
        ++cdptr;
    }

    EXPECT_TRUE(it == contents.end());
    EXPECT_TRUE(cit == contents.cend());
    EXPECT_TRUE(cit2 == cref.end());

    auto rit = contents.rbegin();
    auto crit = contents.crbegin();
    auto crit2 = cref.rbegin();
    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(*rit, 10 - i - 1);
        EXPECT_EQ(*crit, 10 - i - 1);
        EXPECT_EQ(*crit2, 10 - i - 1);
        ++rit;
        ++crit;
        ++crit2;
    }

    EXPECT_TRUE(rit == contents.rend());
    EXPECT_TRUE(crit == contents.crend());
    EXPECT_TRUE(crit2 == cref.rend());

    EXPECT_EQ(contents.front(), 0);
    EXPECT_EQ(contents.back(), 9);
    EXPECT_EQ(cref.front(), 0);
    EXPECT_EQ(cref.back(), 9);

    contents[0] = 100;
    EXPECT_EQ(contents[0], 100);
    EXPECT_EQ(cref[0], 100);
}

TEST(Vector, Resizing) {
    raiigraph::initialize();

    {
        raiigraph::IntegerVector contents(10);
        contents.clear();
        EXPECT_EQ(contents.size(), 0);
    }

    // Resizing works as expected.
    {
        raiigraph::IntegerVector contents;
        contents.resize(10, 1);
        EXPECT_EQ(contents.front(), 1);
        EXPECT_EQ(contents.back(), 1);
        EXPECT_EQ(contents.size(), 10);

        contents.resize(11, 2);
        EXPECT_EQ(contents.front(), 1);
        EXPECT_EQ(contents.back(), 2);
        EXPECT_EQ(contents.size(), 11);

        contents.resize(10);
        EXPECT_EQ(contents.front(), 1);
        EXPECT_EQ(contents.back(), 1);
        EXPECT_EQ(contents.size(), 10);
    }

    // Pushing back and popping back works.
    {
        raiigraph::IntegerVector contents(10, 1);
        contents.push_back(2);
        EXPECT_EQ(contents.back(), 2);
        EXPECT_EQ(contents.size(), 11);

        contents.pop_back();
        EXPECT_EQ(contents.back(), 1);
        EXPECT_EQ(contents.size(), 10);
    }

    // Capacity changes are respected.
    {
        raiigraph::IntegerVector contents(10);

        contents.reserve(1000);
        EXPECT_EQ(contents.size(), 10);
        EXPECT_EQ(contents.capacity(), 1000);

        contents.shrink_to_fit();
        EXPECT_EQ(contents.size(), 10);
        EXPECT_LT(contents.capacity(), 1000);
    }

    // Erasure works as expected.
    {
        raiigraph::IntegerVector contents(10);
        std::iota(contents.begin(), contents.end(), 0);
        auto out = contents.erase(contents.begin() + 5);
        EXPECT_EQ(contents.size(), 9);
        EXPECT_EQ(*out, 6);
        EXPECT_EQ(contents[5], 6);
        EXPECT_EQ(contents[4], 4);
        EXPECT_EQ(contents[6], 7);

        auto out2 = contents.erase(contents.begin() + 5, contents.end());
        EXPECT_EQ(contents.size(), 5);
        EXPECT_TRUE(out2 == contents.end());
        EXPECT_EQ(contents[4], 4);
    }

    // Insertion works as expected.
    {
        raiigraph::IntegerVector contents(10, 1);
        auto out = contents.insert(contents.begin() + 5, 2);
        EXPECT_EQ(*out, 2);
        EXPECT_EQ(contents[4], 1);
        EXPECT_EQ(contents[5], 2);
        EXPECT_EQ(contents[6], 1);
        EXPECT_EQ(contents.size(), 11);

        auto out2 = contents.insert(contents.begin() + 6, 2, 3);
        EXPECT_EQ(*out2, 3);
        EXPECT_EQ(contents[4], 1);
        EXPECT_EQ(contents[5], 2);
        EXPECT_EQ(contents[6], 3);
        EXPECT_EQ(contents[7], 3);
        EXPECT_EQ(contents[8], 1);
        EXPECT_EQ(contents.size(), 13);

        std::vector<double> vals{ 4, 5, 6 };
        auto out3 = contents.insert(contents.begin() + 8, vals.begin(), vals.end());
        EXPECT_EQ(*out3, 4);
        EXPECT_EQ(contents[4], 1);
        EXPECT_EQ(contents[5], 2);
        EXPECT_EQ(contents[6], 3);
        EXPECT_EQ(contents[7], 3);
        EXPECT_EQ(contents[8], 4);
        EXPECT_EQ(contents[9], 5);
        EXPECT_EQ(contents[10], 6);
        EXPECT_EQ(contents[11], 1);
        EXPECT_EQ(contents.size(), 16);
    }
}

TEST(Vector, Coercion) {
    raiigraph::initialize();

    raiigraph::IntegerVector contents(10);
    EXPECT_EQ(contents.get(), static_cast<igraph_vector_int_t*>(contents));

    const auto& cref = contents;
    EXPECT_EQ(cref.get(), static_cast<const igraph_vector_int_t*>(cref));
}

TEST(Vector, Swap) {
    raiigraph::initialize();

    raiigraph::IntegerVector contents(10);
    raiigraph::IntegerVector contents2(5, 1);

    contents.swap(contents2);
    EXPECT_EQ(contents.size(), 5);
    EXPECT_EQ(contents[0], 1);
    EXPECT_EQ(contents2.size(), 10);
    EXPECT_EQ(contents2[0], 0);
}

TEST(Vector, OtherTypes) {
    raiigraph::initialize();

    raiigraph::BoolVector bcontents(1, 0);
    EXPECT_FALSE(bcontents.front());

    raiigraph::RealVector rcontents(1, 10.5);
    EXPECT_EQ(rcontents.front(), 10.5);
}
