#ifndef RAIIGRAPH_INITIALIZE_HPP
#define RAIIGRAPH_INITIALIZE_HPP

#include "error.hpp"

/**
 * @file initialize.hpp
 * @brief Initialize the **igraph** library.
 */

namespace raiigraph {

/**
 * Initialize the **igraph** library by calling `igraph_setup()`.
 * This should be called before any **igraph** functions or **raiigraph** classes are used.
 *
 * @return Boolean indicating whether initialization has already been performed.
 * If `true`, this function is a no-op.
 */
inline bool initialize() {
    static bool initialized = false;
    if (initialized) {
        return true;
    } 

    const auto stat = igraph_setup();
    check_code(stat);
    return false;
}

}

#endif
