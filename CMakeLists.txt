cmake_minimum_required(VERSION 3.14)

project(raiigraph
    VERSION 1.1.0
    DESCRIPTION "Wrappers for igraph data structures in C++"
    LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Library
add_library(raiigraph INTERFACE)
add_library(ltla::raiigraph ALIAS raiigraph)

target_include_directories(raiigraph INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/raiigraph>)
target_compile_features(raiigraph INTERFACE cxx_std_17)

# Dependencies
option(RAIIGRAPH_FIND_IGRAPH "Search for the igraph package." ON)
if(RAIIGRAPH_FIND_IGRAPH)
    find_package(igraph 0.10.0 CONFIG REQUIRED)
    target_link_libraries(raiigraph INTERFACE igraph::igraph)
endif()

# Tests
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    option(RAIIGRAPH_TESTS "Build raiigraph's test suite." ON)
else()
    option(RAIIGRAPH_TESTS "Build raiigraph's test suite." OFF)
endif()

if(RAIIGRAPH_TESTS)
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(tests)
    endif() 
endif()

# Install
install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/raiigraph)

install(TARGETS raiigraph
    EXPORT raiigraphTargets)

install(EXPORT raiigraphTargets
    FILE ltla_raiigraphTargets.cmake
    NAMESPACE ltla::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_raiigraph)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_raiigraphConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_raiigraph)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_raiigraphConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ltla_raiigraphConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/ltla_raiigraphConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ltla_raiigraph)
