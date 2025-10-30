#ifndef RAIIGRAPH_INITIALIZE_HPP
#define RAIIGRAPH_INITIALIZE_HPP

/**
 * @file initialize.hpp
 * @brief Initial setup for the **igraph** library.
 */

namespace raiigraph {

/**
 * Sets up the **igraph** library.
 * This should be performed before any **igraph** functionality is used, but only needs to be done once for the lifetime of the program.
 * `initialize()` is automatically called by any **raiigraph** constructor but can also be called manually.
 *
 * @return Boolean indicating whether `initialize()` had been previously called.
 * If true, this function was a no-op.
 */
inline bool initialize() {
    static bool is_initialized = false;
    if (is_initialized) {
        return true;
    }
    igraph_setup();
    is_initialized = true;
    return false;
}

}

#endif
