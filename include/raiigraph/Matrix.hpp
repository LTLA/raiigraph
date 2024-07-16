#ifndef RAIIGRAPH_MATRIX_HPP
#define RAIIGRAPH_MATRIX_HPP

#include "igraph.h"
#include "error.hpp"
#include <algorithm>
#include <iterator>

namespace raiigraph {

/**
 * @brief Wrapper around `igraph_matrix_*_t` objects with RAII semantics.
 * @tparam Ns_ Structure-based namespace with static methods, internal use only.
 */
template<class Ns_>
class Matrix {
private:
    void setup(igraph_integer_t nr, igraph_integer_t nc) {
        if (Ns_::init(&my_matrix, nr, nc)) {
            throw std::runtime_error("failed to initialize igraph matrix of dimensions " + std::to_string(nr) + " x " + std::to_string(nc));
        }
    }

public:
    /**
     * Type of the underlying **igraph** matrix.
     */
    typedef typename Ns_::igraph_type igraph_type;

    /**
     * Type of the values inside the matrix.
     */
    typedef typename Ns_::value_type value_type;

    /**
     * Type of the reference to values inside the matrix.
     */
    typedef value_type& reference;

    /**
     * Type of a const reference to values inside the matrix.
     */
    typedef const value_type& const_reference;

    /**
     * Integer type for the size of the matrix.
     */
    typedef igraph_integer_t size_type;

    /**
     * Integer type for differences in positions within the matrix.
     */
    typedef igraph_integer_t difference_type;

    /**
     * Iterator for the matrix contents. 
     */
    typedef value_type* iterator;

    /**
     * Const iterator for the matrix contents. 
     */
    typedef const value_type* const_iterator;

    /**
     * Reverse iterator for the matrix contents.
     */
    typedef std::reverse_iterator<iterator> reverse_iterator;

    /**
     * Reverse const iterator for the matrix contents.
     */
    typedef std::reverse_iterator<const_iterator> reverse_const_iterator;

public:
    /**
     * Default constructor, creates an initialized matrix with no rows or columns.
     */
    Matrix() : Matrix(0, 0) {}

    /**
     * @param nr Number of rows in the matrix.
     * @param nc Number of columns in the matrix.
     * @param val Value to use to fill the matrix.
     */
    Matrix(size_type nr, size_type nc, const value_type& val = value_type()) {
        setup(nr, nc);
        if (val != 0) { // setup() already zero-initializes the backing array.
            std::fill(begin(), end(), val);
        }
    }

    /**
     * @param matrix An initialized matrix to take ownership of.
     */
    Matrix(igraph_type&& matrix) : my_matrix(std::move(matrix)) {}

public:
    /**
     * @param other Matrix to be copy-constructed from.
     * This constructor will make a deep copy.
     */
    Matrix(const Matrix<Ns_>& other) {
        if (Ns_::copy(&my_matrix, &(other.my_matrix))) {
            throw std::runtime_error("failed to copy-construct integer igraph matrix");
        }
    }

    /**
     * @param other Matrix to be copy-assigned from.
     * This constructor will make a deep copy.
     */
    Matrix<Ns_>& operator=(const Matrix<Ns_>& other) {
        if (this != &other) {
            if (Ns_::update(&my_matrix, &(other.my_matrix))) { // my_matrix should already be initialized before the assignment.
                throw std::runtime_error("failed to copy-assign integer igraph matrix");
            }
        }
        return *this;
    }

    /**
     * @param other Matrix to be move-constructed from.
     * This constructor will leave `other` in a valid but unspecified state.
     */
    Matrix(Matrix<Ns_>&& other) {
        setup(0); // we must leave 'other' in a valid state.
        std::swap(my_matrix, other.my_matrix);
    }

    /**
     * @param other Matrix to be move-assigned from.
     * This constructor will leave `other` in a valid but unspecified state.
     */
    Matrix& operator=(Matrix<Ns_>&& other) {
        if (this != &other) {
            std::swap(my_matrix, other.my_matrix); // 'my_matrix' should already be initialized, so we're leaving 'other' in a valid state.
        }
        return *this;
    }

    /**
     * Destructor.
     */
    ~Matrix() {
        Ns_::destroy(&my_matrix);
    }

public:
    /**
     * @return Whether the matrix is empty.
     */
    igraph_bool_t empty() const {
        return Ns_::empty(&my_matrix);
    }

    /**
     * @return Size of the matrix.
     */
    size_type size() const {
        return Ns_::size(&my_matrix);
    }

    /**
     * @return Number of rows in the matrix.
     */
    size_type nrow() const {
        return my_matrix.nrow; 
    }

    /**
     * @return Number of columns in the matrix.
     */
    size_type ncol() const {
        return my_matrix.ncol;
    }

    /**
     * @return Maximum size of this matrix.
     */
    constexpr size_type max_size() const { return IGRAPH_INTEGER_MAX; }

    /**
     * @return Capacity of this matrix.
     */
    size_type capacity() const {
        return my_matrix.data.stor_end - my_matrix.data.stor_begin;
    }

public:
    /**
     * Clear this matrix, leaving it with a size of zero.
     */
    void clear() {
        resize(0, 0);
    }

    /**
     * Resize the matrix to the specified number of rows and columns.
     * @param nr New number of rows.
     * @param nr New number of columns.
     * @param val Value to use to fill the new elements, if `size` is greater than the current size.
     */
    void resize(size_type nr, size_type nc, value_type val = value_type()) {
        auto old_size = this->size();
        check_code(Ns_::resize(&my_matrix, nr, nc));
        auto new_size = this->size();
        if (old_size < new_size) {
            std::fill_n(begin() + old_size, new_size - old_size, val);
        }
    }

    /**
     * Shrink the capacity of the matrix to fit the contents.
     */
    void shrink_to_fit() {
        Ns_::shrink_to_fit(&my_matrix);
    }

public:
    /**
     * @param i Index on the matrix.
     * @return Reference to the value at `i`.
     */
    reference operator[](size_type i) {
        return *(begin() + i);
    }

    /**
     * @param i Index on the matrix.
     * @return Const reference to the value at `i`.
     */
    const_reference operator[](size_type i) const {
        return *(begin() + i);
    }

    /**
     * @param r Row of interest.
     * @param c Column of interest.
     * @return Reference to the value at `(r, c)`.
     */
    reference operator()(size_type r, size_type c) {
        return *(begin() + r + c * my_matrix.nrow); // no need to worry about overflow as size_type is guaranteed to hold the full size.
    }

    /**
     * @param r Row of interest.
     * @param c Column of interest.
     * @return Const reference to the value at `(r, c)`.
     */
    const_reference operator()(size_type r, size_type c) const {
        return *(begin() + r + c * my_matrix.nrow);
    }

    /**
     * @return Reference to the last element in the matrix.
     */
    reference back() {
        return *(end() - 1);
    }

    /**
     * @return Const reference to the last element in the matrix.
     */
    const_reference back() const {
        return *(end() - 1);
    }

    /**
     * @return Reference to the last element in the matrix.
     */
    reference front() {
        return *(begin());
    }

    /**
     * @return Const reference to the last element in the matrix.
     */
    const_reference front() const {
        return *(begin());
    }

public:
    /**
     * @return Iterator to the start of this matrix.
     */
    iterator begin() {
        return my_matrix.data.stor_begin;
    }

    /**
     * @return Iterator to the end of this matrix.
     */
    iterator end() {
        return my_matrix.data.stor_end;
    }

    /**
     * @return Const iterator to the start of this matrix.
     */
    const_iterator begin() const {
        return cbegin();
    }

    /**
     * @return Const iterator to the end of this matrix.
     */
    const_iterator end() const {
        return cend();
    }

    /**
     * @return Const iterator to the start of this matrix.
     */
    const_iterator cbegin() const {
        return my_matrix.data.stor_begin;
    }

    /**
     * @return Const iterator to the end of this matrix.
     */
    const_iterator cend() const {
        return my_matrix.data.stor_end;
    }

    /**
     * @return Pointer to the start of this matrix.
     */
    value_type* data() {
        return my_matrix.data.stor_begin;
    }

    /**
     * @return Const pointer to the start of this matrix.
     */
    const value_type* data() const {
        return my_matrix.data.stor_begin;
    }

    /**
     * @return Reverse iterator to the last element of this matrix.
     */
    reverse_iterator rbegin() {
        return std::reverse_iterator(end());
    }

    /**
     * @return Reverse iterator to a location before the start of this matrix.
     */
    reverse_iterator rend() {
        return std::reverse_iterator(begin());
    }

    /**
     * @return Reverse const iterator to the last element of this matrix.
     */
    reverse_const_iterator rbegin() const {
        return std::reverse_iterator(end());
    }

    /**
     * @return Reverse const iterator to a location before the start of this matrix.
     */
    reverse_const_iterator rend() const {
        return std::reverse_iterator(begin());
    }

    /**
     * @return Reverse const iterator to the last element of this matrix.
     */
    reverse_const_iterator crbegin() const {
        return std::reverse_iterator(cend());
    }

    /**
     * @return Reverse const iterator to a location before the start of this matrix.
     */
    reverse_const_iterator crend() const {
        return std::reverse_iterator(cbegin());
    }

public:
    /**
     * @brief A `Vector`-like view into a row/column of the matrix.
     *
     * Note that views are potentially invalidated by any re/deallocations in the parent `Matrix`. 
     */
    template<typename BaseIterator, typename BaseReference>
    class View {
    /**
     * @cond
     */
    public:
        View(BaseIterator start, size_type step_size, size_type max_steps) : start(start), step_size(step_size), max_steps(max_steps) {}

    private:
        BaseIterator start;
        size_type step_size, max_steps;
    /**
     * @endcond
     */

    public:
        /**
         * @return Whether the row vector is empty.
         */
        bool empty() const {
            return max_steps == 0;
        }

        /**
         * @return Length of the row vector.
         */
        size_type size() const {
            return max_steps;
        }

        /**
         * @param i Index on the row vector (i.e., the column).
         * @return Reference to the value at `i`.
         */
        BaseReference operator[](size_type i) const {
            return *(start + i * step_size); // no need to cast to avoid overflow, as size_type determines the max size anyway.
        }

        /**
         * @return Reference to the last element in the row vector.
         */
        BaseReference back() const {
            return (*this)[max_steps - 1];
        }

        /**
         * @return Reference to the first element in the row vector.
         */
        BaseReference front() const {
            return *start;
        }

    public:
        /**
         * @brief Random-access iterator through the row/column vector.
         */
        struct Iterator {
        private:
            iterator start;
            size_type step_size = 0;

            // Note that we don't just shift 'start' directly as defining the
            // 'end()' iterator of the view could up shifting 'start' to an
            // address beyond the one-past-the-end of the backing array in the
            // 'igraph_matrix_t'. Pointer arithmetic beyond the array bounds is
            // undefined IIRC, so instead, we store the offset and att it to
            // 'start' upon dereference of the iterator. This ensures that an
            // invalid address is never constructed even if it is unused.
            size_type offset = 0; 
        public:
            /**
             * @cond
             */
            // List of required methods taken from https://cplusplus.com/reference/iterator/RandomAccessIterator/.
            using iterator_category = std::random_access_iterator_tag;

            explicit Iterator(iterator start, size_type step_size, size_type position) : 
                start(std::move(start)), step_size(step_size), offset(position * step_size) {}

            Iterator() = default;

        public:
            bool operator==(const Iterator& other) const { 
                // see comments at https://stackoverflow.com/questions/4657513/comparing-iterators-from-different-containers
                // regarding the UB of comparing iterators from different containers, so we'll just quietly ignore it.
                return offset == other.offset;  
            }

            bool operator!=(const Iterator& other) const { 
                return offset != other.offset;
            }

            bool operator<(const Iterator& other) const {
                return offset < other.offset;
            }

            bool operator>(const Iterator& other) const {
                return offset > other.offset;
            }

            bool operator<=(const Iterator& other) const {
                return offset <= other.offset;
            }

            bool operator>=(const Iterator& other) const {
                return offset >= other.offset;
            }

        public:
            BaseReference operator*() const {
                return *(start + offset);
            }

            BaseReference operator[](size_type i) const {
                return *(start + offset + step_size * i);
            }

        public:
            Iterator& operator++() { 
                offset += step_size;
                return *this; 
            }

            Iterator operator++(int) { 
                auto copy = *this;
                offset += step_size;
                return copy;
            }

            Iterator& operator--() { 
                offset -= step_size;
                return *this; 
            }

            Iterator operator--(int) { 
                auto copy = *this;
                offset -= step_size;
                return copy;
            }

            Iterator operator+(difference_type n) const { 
                auto copy = *this;
                copy.offset += step_size * n;
                return copy;
            }

            Iterator& operator+=(difference_type n) { 
                offset += step_size * n;
                return *this; 
            }

            friend Iterator operator+(difference_type n, Iterator it) {
                it.offset += it.step_size * n;
                return it;
            }

            Iterator operator-(difference_type n) const { 
                auto copy = *this;
                copy.offset -= step_size * n;
                return copy;
            }

            Iterator& operator-=(difference_type n) { 
                offset -= step_size * n;
                return *this;
            }

            difference_type operator-(const Iterator& other) const {
                if (other.offset > offset) {
                    difference_type delta = (other.offset - offset) / step_size;
                    return -delta;
                } else {
                    return (offset - other.offset) / step_size;
                }
            }
            /**
             * @endcond
             */
        };

        /**
         * @return Iterator to the start of this row vector.
         */
        Iterator begin() const {
            return Iterator(start, step_size, 0);
        }

        /**
         * @return Iterator to the end of this row vector.
         */
        Iterator end() const {
            return Iterator(start, step_size, max_steps);
        }

        /**
         * @return Const iterator to the start of this row vector.
         */
        Iterator cbegin() const {
            return begin();
        }

        /**
         * @return Const iterator to the end of this row vector.
         */
        Iterator cend() const {
            return end();
        }

        /**
         * @return Reverse const iterator to the last element of this row vector.
         */
        std::reverse_iterator<Iterator> rbegin() const {
            return std::reverse_iterator(end());
        }

        /**
         * @return Reverse const iterator to a location before the start of this row vector.
         */
        std::reverse_iterator<Iterator> rend() const {
            return std::reverse_iterator(begin());
        }

        /**
         * @return Reverse const iterator to the last element of this row vector.
         */
        std::reverse_iterator<Iterator> crbegin() const {
            return std::reverse_iterator(cend());
        }

        /**
         * @return Reverse const iterator to a location before the start of this row vector.
         */
        std::reverse_iterator<Iterator> crend() const {
            return std::reverse_iterator(cbegin());
        }
    };

    /**
     * @param r Row of interest.
     * @return A view on the row.
     */
    View<iterator, reference> row(size_type r) {
        return View<iterator, reference>(begin() + r, my_matrix.ncol);
    }

    /**
     * @param r Row of interest.
     * @return A const view on the row.
     */
    View<const_iterator, const_reference> row(size_type r) const {
        return View<const_iterator, const_reference>(begin() + r, my_matrix.ncol);
    }

    /**
     * @param c Column of interest.
     * @return A view on the column.
     */
    View<iterator, reference> column(size_type c) {
        return View<iterator, reference>(begin() + c * my_matrix.nrow, 1);
    }

    /**
     * @param r Column of interest.
     * @return A const view on the column.
     */
    View<const_iterator, const_reference> column(size_type c) const {
        return View<const_iterator, const_reference>(begin() + c * my_matrix.nrow, 1);
    }

public:
    /**
     * @return Pointer to the underlying **igraph** matrix object.
     * This is guaranteed to be non-NULL and initialized.
     */
    operator igraph_type*() {
        return &my_matrix;
    }

    /**
     * @return Const pointer to the underlying **igraph** matrix object.
     * This is guaranteed to be non-NULL and initialized.
     */
    operator const igraph_type*() const {
        return &my_matrix;
    }

    /**
     * @return Pointer to the underlying **igraph** matrix object.
     * This is guaranteed to be non-NULL and initialized.
     */
    igraph_type* get() {
        return &my_matrix;
    }

    /**
     * @return Const pointer to the underlying **igraph** matrix object.
     * This is guaranteed to be non-NULL and initialized.
     */
    const igraph_type* get() const {
        return &my_matrix;
    }

public:
    /**
     * Swap two matrixs, maintaining the validity of existing pointers to each matrix and its elements.
     * @param other Matrix to be swapped.
     */
    void swap(Matrix<Ns_>& other)  {
        // Swapping structures entirely to ensure that iterators and pointers
        // remain valid; looks like igraph_matrix_swap does the same.
        std::swap(my_matrix, other.my_matrix);
    }

private:
    igraph_type my_matrix;
};

/**
 * @cond
 */
namespace internal {

struct Integer {
    typedef igraph_integer_t value_type;
    typedef igraph_matrix_int_t igraph_type;

#define RAIIGRAPH_MATRIX_SUFFIX _int
#include "fragments/matrix.hpp"
#undef RAIIGRAPH_MATRIX_SUFFIX
};

struct Real {
    typedef igraph_real_t value_type;
    typedef igraph_matrix_t igraph_type;

#define RAIIGRAPH_MATRIX_SUFFIX
#include "fragments/matrix.hpp"
#undef RAIIGRAPH_MATRIX_SUFFIX
};

struct Bool {
    typedef igraph_bool_t value_type;
    typedef igraph_matrix_bool_t igraph_type;

#define RAIIGRAPH_MATRIX_SUFFIX _bool
#include "fragments/matrix.hpp"
#undef RAIIGRAPH_MATRIX_SUFFIX
};

}
/**
 * @endcond
 */

/**
 * Matrix of **igraph** integers.
 */
typedef Matrix<internal::Integer> IntMatrix;

/**
 * @cond
 */
// For back-compatibility.
typedef IntMatrix IntegerMatrix;
/**
 * @endcond
 */

/**
 * Matrix of **igraph** reals.
 */
typedef Matrix<internal::Real> RealMatrix;

/**
 * Matrix of **igraph** booleans.
 */
typedef Matrix<internal::Bool> BoolMatrix;

}


#endif