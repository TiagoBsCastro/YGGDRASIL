#pragma once
#include <cstdlib>
#include <vector>

/**
 * @brief Perform friends-of-friends clustering using an R-tree.
 *
 * This template function performs spatial clustering using a friends-of-friends algorithm,
 * leveraging Boost Geometry's R-tree for efficient spatial indexing and querying. It clusters points
 * that are within a specified linking length of each other. The function is generic and works
 * for any specified dimension D.
 *
 * @tparam D Dimensionality of the space in which the points exist.
 * @param data Pointer to the array of point coordinates, assumed to be contiguous in memory.
 * @param npts Number of points in the data array.
 * @param linking_length Maximum distance between points to be considered part of the same cluster.
 * @return std::vector<std::vector<size_t>> A vector of clusters, each represented as a vector of point indices.
 */
std::vector< std::vector<std::size_t> >  friends_of_friends(double* data, std::size_t npts, std::size_t ndim, double linking_length);
