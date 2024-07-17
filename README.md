# C++ RAII for igraph data structures

![Unit tests](https://github.com/LTLA/raiigraph/actions/workflows/run-tests.yaml/badge.svg)
![Documentation](https://github.com/LTLA/raiigraph/actions/workflows/doxygenate.yaml/badge.svg)
[![Codecov](https://codecov.io/gh/LTLA/raiigraph/graph/badge.svg?token=qiFzL0PTBw)](https://codecov.io/gh/LTLA/raiigraph)

## Overview 

This library provides some RAII wrappers around the data structures used by the [**igraph** C library](https://igraph.org).
The aim is to make **igraph** a little bit more ergonomic for use in existing C++ codebases.
The code here was originally developed as part of the [**libscran**](https://github.com/LTLA/libscran) library,
but has been factored out for potential re-use.

Currently, we implement wrappers for the following **igraph** structures:

| **igraph** structure | **raiigraph** wrapper |
|----------------------|-----------------------|
| `igraph_vector_t`    | `raiigraph::RealVector` |
| `igraph_vector_int_t` | `raiigraph::IntVector` |
| `igraph_vector_bool_t` | `raiigraph::BoolVector` |
| `igraph_matrix_t` | `raiigraph::RealMatrix` |
| `igraph_matrix_int_t` | `raiigraph::IntMatrix` |
| `igraph_matrix_bool_t` | `raiigraph::BoolMatrix` |
| `igraph_t` | `raiigraph::Graph` |

Please make a PR if your favorite structure is missing.

## Quick start

All wrappers provide the usual RAII behavior, greatly simplifying memory management when used in existing code.

```cpp
{
    raiigraph::IntVector thing(50);
    igraph_vector_int_size(thing); // can directly use the object with igraph.
} // thing's memory is released when 'thing' goes out of scope.
``` 

We also follow the rule of 5 convention, so copy/move assignment works as expected:

```cpp
raiigraph::Graph outside;

{
    raiigraph::Graph inside(20, IGRAPH_UNDIRECTED);
    igraph_graph_ecount(inside); // can directly use the object with igraph.
    outside = std::move(inside);
}
```

Some of the wrappers also provide convenience methods to query the underlying data structure without having to extract the raw pointer:

```cpp
raiigraph::Graph example(20, IGRAPH_UNDIRECTED);
example.ecount(); // number of edges
example.vcount(); // number of vertices
```

Check out the [reference documentation](https://ltla.github.io/raiigraph) for more details.

## STL compatibility

The `Vector` classes provide the same methods as `std::vector` and can be used as a drop-in replacement in various standard library functions:

```cpp
// Vector provides std::vector-like behavior.
raiigraph::RealVector vec(10);
thing[0] = 1;
thing.push_back(5);
std::fill(thing.begin() + 2, thing.begin() + 5, 10);
std::sort(thing.begin(), thing.end());
auto& last = thing.back();
```

The `Matrix` classes provide views into the rows and columns that have STL-like behavior.

```cpp
raiigraph::IntMatrix mat(10, 20);
auto row_view = mat.row(5); // i.e., 5th row.
row_view.size();
row_view[2] = 5; // modifies the underlying matrix.
row_view.back() = 10;
std::sort(row_view.begin(), row_view.end());
```

## Controlling the RNG

The `RNGScope` class allows users to easily set the **igraph** RNG for reproducible execution.
This uses RAII to restore the previous RNG when the instance goes out of scope:

```cpp
{
    RNGScope scope(10); // sets the RNG with seed of 10.
    // use igraph functions that need the RNG.
} // restores the previous RNG.
```

This class can be used in a nested manner for easy composition:

```cpp
void foo() {
    RNGScope scope(20); // sets the RNG with seed of 20.
    // use igraph functions that need the RNG.
} // restores the previous RNG.

{
    RNGScope scope(10); // sets the RNG with seed of 10.

    if (maybe_run) {
        foo();
    }

    // use igraph functions that need the RNG, getting the same results
    // regardless of whether foo() was called.
} // restores the previous RNG.
```

## Building projects

### CMake with `FetchContent`

If you're using CMake, you just need to add something like this to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  raiigraph
  GIT_REPOSITORY https://github.com/LTLA/raiigraph
  GIT_TAG master # or any version of interest
)

FetchContent_MakeAvailable(raiigraph)
```

Then you can link to **raiigraph** to make the headers available during compilation:

```cmake
# For executables:
target_link_libraries(myexe raiigraph::raiigraph)

# For libaries
target_link_libraries(mylib INTERFACE raiigraph::raiigraph)
```

This assumes that **igraph** has already been installed and can be found by `find_package()`. 
Users can use the `RAIIGRAPH_FIND_IGRAPH` option to disable **igraph** discovery (e.g., to supply a custom **igraph** installation),
in which case they will need to link to **igraph** manually in their `target_link_libraries()` call.

### CMake with `find_package()`

```cmake
find_package(raiigraph_raiigraph CONFIG REQUIRED)
target_link_libraries(mylib INTERFACE ltla::raiigraph)
```

To install the library, use:

```sh
mkdir build && cd build
cmake .. -DKNNCOLLE_TESTS=OFF
cmake --build . --target install
```

Again, this assumes that **igraph** has already been installed and can be found by `find_package()`, unless otherwise specified via `RAIIGRAPH_FIND_IGRAPH`.

### Manual

If you're not using CMake, the simple approach is to just copy the files in `include/` - either directly or with Git submodules - and include their path during compilation with, e.g., GCC's `-I`.
This means that you are responsible for manually linking to **igraph**. 
