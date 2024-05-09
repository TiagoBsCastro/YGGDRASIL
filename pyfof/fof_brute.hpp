#pragma once

#include <vector>

/**
 * @brief Brute force implementation to find friends-of-friends clusters.
 *
 * This function implements the friends-of-friends (FoF) clustering algorithm,
 * which groups points that are within a specified linking length of each other.
 * It is a brute force method, meaning it compares each point against every other
 * point to determine clusters, which can be computationally intensive.
 *
 * @param data Pointer to the array of data points, where each point has 'ndim' dimensions.
 * @param npts The total number of points in the dataset.
 * @param ndim The number of dimensions each point has.
 * @param linking_length The maximum distance between two points to consider them as "friends".
 * @return std::vector<std::vector<size_t>> A list of clusters, with each cluster being a list
 *         of indices representing points that are grouped together.
 */
std::vector< std::vector<std::size_t> > friends_of_friends_brute(double* data, std::size_t npts, std::size_t ndim, double linking_length);
