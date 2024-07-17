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

    raiigraph::IntMatrix imat(50, 20);
    EXPECT_FALSE(imat.empty());
    EXPECT_EQ(imat.size(), 1000);
    EXPECT_EQ(imat.nrow(), 50);
    EXPECT_EQ(imat.ncol(), 20);
    EXPECT_FALSE(imat.get() == NULL);

    // Trying copy construction/assignment to get some coverage.
    {
        raiigraph::IntMatrix imat2(imat);
        EXPECT_EQ(imat2.size(), 1000);
        EXPECT_FALSE(imat2.get() == imat.get());

        igraph_matrix_int_set(imat2, 0, 0, 10);
        EXPECT_EQ(igraph_matrix_int_get(imat2, 0, 0), 10);
        EXPECT_EQ(igraph_matrix_int_get(imat, 0, 0), 0); // doesn't affect the original.

        raiigraph::IntMatrix imat3;
        imat3 = imat2;
        EXPECT_EQ(imat3.size(), 1000);
        EXPECT_FALSE(imat3.get() == imat2.get());

        igraph_matrix_int_set(imat3, 0, 0, 20);
        EXPECT_EQ(igraph_matrix_int_get(imat3, 0, 0), 20);
        EXPECT_EQ(igraph_matrix_int_get(imat2, 0, 0), 10); // doesn't affect the original.
    }

    // Trying move construction/assignment to get some coverage.
    {
        auto copy = imat;
        raiigraph::IntMatrix imat2(std::move(copy));
        EXPECT_EQ(imat2.size(), 1000);
        EXPECT_NE(copy.size(), 1000); // moved from...
        EXPECT_FALSE(imat2.get() == imat.get());

        raiigraph::IntMatrix imat3;
        imat3 = std::move(imat2);
        EXPECT_EQ(imat3.size(), 1000);
        EXPECT_NE(imat2.size(), 1000); // moved from...
        EXPECT_FALSE(imat3.get() == imat2.get());
    }

    // Taking ownership.
    {
        igraph_matrix_int_t tmp;
        igraph_matrix_int_init(&tmp, 100, 10);
        raiigraph::IntMatrix owner(std::move(tmp));
        EXPECT_EQ(owner.size(), 1000);
    }
}

TEST(Matrix, Access1D) {
    raiigraph::IntegerMatrix contents(10, 20); 
    std::iota(contents.begin(), contents.end(), 0); // column-major filling.

    const auto& cref = contents;

    auto it = contents.begin();
    auto cit = contents.cbegin();
    auto cit2 = cref.begin();
    auto dptr = contents.data();
    auto cdptr = cref.data();
    for (int i = 0; i < 200; ++i) {
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
    for (int i = 0; i < 200; ++i) {
        EXPECT_EQ(*rit, 200 - i - 1);
        EXPECT_EQ(*crit, 200 - i - 1);
        EXPECT_EQ(*crit2, 200 - i - 1);
        ++rit;
        ++crit;
        ++crit2;
    }

    EXPECT_TRUE(rit == contents.rend());
    EXPECT_TRUE(crit == contents.crend());
    EXPECT_TRUE(crit2 == cref.rend());

    EXPECT_EQ(contents.front(), 0);
    EXPECT_EQ(contents.back(), 199);
    EXPECT_EQ(cref.front(), 0);
    EXPECT_EQ(cref.back(), 199);

    contents[0] = 100;
    EXPECT_EQ(contents[0], 100);
    EXPECT_EQ(cref[0], 100);
}

TEST(Matrix, Access2D) {
    raiigraph::IntegerMatrix contents(10, 20); 
    std::iota(contents.begin(), contents.end(), 0); // column-major filling.

    for (int j = 0; j < 20; ++j) {
        for (int i = 0; i < 10; ++i) {
            EXPECT_EQ(contents(i, j), i + 10 * j);
        }
    }

    for (int j = 0; j < 20; ++j) {
        for (int i = 0; i < 10; ++i) {
            contents(i, j) = 0;
        }
    }

    for (auto x : contents) {
        EXPECT_EQ(x, 0);
    }
}

TEST(Matrix, AccessRow) {
    raiigraph::IntegerMatrix contents(10, 20); 
    std::iota(contents.begin(), contents.end(), 0); // column-major filling.
    const auto& const_contents = contents;

    for (int i = 0; i < 10; ++i) {
        // Checking a simple copy.
        auto copy = contents.row_copy(i);
        EXPECT_EQ(copy.front(), i);
        EXPECT_EQ(copy.back(), i + 190);

        // Checking the views.
        auto row = contents.row(i);
        EXPECT_EQ(row.size(), 20);
        EXPECT_FALSE(row.empty());
        EXPECT_EQ(row.front(), i);
        EXPECT_EQ(row.back(), i + 190);

        auto cref = const_contents.row(i);
        EXPECT_EQ(cref.front(), i);
        EXPECT_EQ(cref.back(), i + 190);

        // Check the forward iterators.
        auto it = row.begin();
        auto cit = row.cbegin();
        auto cit2 = cref.begin();

        for (int j = 0; j < 20; ++j) {
            int expected = i + j * 10;
            EXPECT_EQ(*it, expected);
            EXPECT_EQ(*cit, expected);
            EXPECT_EQ(*cit2, expected);
            ++it;
            ++cit;
            ++cit2;
        }

        EXPECT_TRUE(it == row.end());
        EXPECT_TRUE(cit == row.cend());
        EXPECT_TRUE(cit2 == cref.end());

        // Check the reverse iterators.
        auto rit = row.rbegin();
        auto crit = row.crbegin();
        auto crit2 = cref.rbegin();
        for (int j = 0; j < 20; ++j) {
            int expected = i + (20 - j - 1) * 10;
            EXPECT_EQ(*rit, expected);
            EXPECT_EQ(*crit, expected);
            EXPECT_EQ(*crit2, expected);
            ++rit;
            ++crit;
            ++crit2;
        }

        EXPECT_TRUE(rit == row.rend());
        EXPECT_TRUE(crit == row.crend());
        EXPECT_TRUE(crit2 == cref.rend());

        // Check all the arithmetic.
        {
            auto it = row.begin();
            EXPECT_EQ(*(it++), i);
            EXPECT_EQ(*it, i + 10);
            EXPECT_EQ(*(++it), i + 20);
            EXPECT_EQ(*(it += 2), i + 40);

            auto it2 = row.end();
            EXPECT_EQ(*(--it2), i + 190);
            EXPECT_EQ(*(it2--), i + 190);
            EXPECT_EQ(*it2, i + 180);
            EXPECT_EQ(*(it2 -= 2), i + 160);

            EXPECT_EQ(*(row.begin() + 5), i + 50);
            EXPECT_EQ(*(10 + row.begin()), i + 100);
            EXPECT_EQ(*(row.end() - 5), i + 150);

            EXPECT_EQ(row.end() - row.begin(), 20);
        }

        // Check the comparison operations.
        {
            auto it = row.begin();
            EXPECT_TRUE(it < row.end());
            EXPECT_TRUE(it != row.end());
            EXPECT_FALSE(it < row.begin());
            EXPECT_TRUE(it <= row.begin());
            EXPECT_FALSE(it > row.end());
            EXPECT_FALSE(it > row.begin());
            EXPECT_TRUE(it >= row.begin());
        }

        // We can also modify contents via the rows or their iterators.
        {
            auto it = row.begin();
            *it = -1;
            row.back() = -1;
            EXPECT_EQ(contents(i, 0), -1);
            EXPECT_EQ(contents(i, 19), -1);

            auto rit = row.rbegin();
            *rit = 20;
            EXPECT_EQ(contents(i, 19), 20);
        }
    }
}

TEST(Matrix, AccessColumn) {
    raiigraph::IntegerMatrix contents(10, 20); 
    std::iota(contents.begin(), contents.end(), 0); // column-major filling.
    const auto& const_contents = contents;

    for (int j = 0; j < 20; ++j) {
        auto copy = contents.column_copy(j);
        EXPECT_EQ(copy.front(), j * 10);
        EXPECT_EQ(copy.back(), j * 10 + 9);

        // Checking the views.
        auto row = contents.column(j);
        EXPECT_EQ(row.size(), 10);
        EXPECT_FALSE(row.empty());
        EXPECT_EQ(row.front(), j * 10);
        EXPECT_EQ(row.back(), j * 10 + 9);

        auto cref = const_contents.column(j);
        EXPECT_EQ(cref.front(), j * 10);
        EXPECT_EQ(cref.back(), j * 10 + 9);

        // This is much the same as rows, so we won't bother doing all the
        // checks; just make sure everything is covered.
        auto it = row.begin();
        auto cit = row.cbegin();
        auto cit2 = cref.begin();

        for (int i = 0; i < 10; ++i) {
            int expected = i + 10 * j; 
            EXPECT_EQ(*it, expected);
            EXPECT_EQ(*cit, expected);
            EXPECT_EQ(*cit2, expected);
            ++it;
            ++cit;
            ++cit2;
        }
    }
}

TEST(Matrix, Coercion) {
    raiigraph::IntegerMatrix contents(10, 10);
    EXPECT_EQ(contents.get(), static_cast<igraph_matrix_int_t*>(contents));

    const auto& cref = contents;
    EXPECT_EQ(cref.get(), static_cast<const igraph_matrix_int_t*>(cref));
}

TEST(Matrix, Swap) {
    raiigraph::IntegerMatrix contents(10, 10);
    raiigraph::IntegerMatrix contents2(5, 5, 1);

    contents.swap(contents2);
    EXPECT_EQ(contents.size(), 25);
    EXPECT_EQ(contents[0], 1);
    EXPECT_EQ(contents2.size(), 100);
    EXPECT_EQ(contents2[0], 0);
}

TEST(Matrix, OtherTypes) {
    raiigraph::BoolMatrix bcontents(1, 1, 0);
    EXPECT_FALSE(bcontents.front());

    raiigraph::RealMatrix rcontents(1, 1, 10.5);
    EXPECT_EQ(rcontents.front(), 10.5);
}
