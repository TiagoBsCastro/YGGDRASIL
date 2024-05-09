#include "fof_brute.hpp"

#include <algorithm>
#include <vector>
#include <cmath>

// Define everything within an anonymous namespace to keep it local to this file.
namespace {

// Define a type alias for size_t for ease of use.
typedef std::size_t size_t;

// Define a Point type as a pair of size_t and a pointer to double,
// representing an index and a pointer to the coordinate data.
typedef std::pair<size_t, double*> Point;

/**
 * @brief Calculate Euclidean distance between two points in n-dimensional space.
 *
 * This function calculates the Euclidean distance between two points represented
 * as arrays of doubles. It is used to determine the spatial distance between two
 * points in an n-dimensional geometric space.
 *
 * @param p1 Pointer to the first point's coordinates.
 * @param p2 Pointer to the second point's coordinates.
 * @param ndim The number of dimensions in which the points exist.
 * @return double The Euclidean distance between the two points.
 */
double dist(double *p1, double *p2, size_t ndim) {
    double d2 = 0.; // Start with a squared distance of 0.
    for(size_t i = 0 ; i < ndim ; ++i) {
        d2 += pow(p1[i] - p2[i], 2); // Sum up the squared differences of coordinates.
    }
    return sqrt(d2); // Return the square root of the summed squares (Euclidean distance).
}

} // End of anonymous namespace

// Brute force implementation to find friends of friends clusters.
std::vector< std::vector<size_t> > friends_of_friends_brute(
    double *data,           // Pointer to the data array.
    size_t npts,            // Number of points in the data.
    size_t ndim,            // Number of dimensions of each point.
    double linking_length   // Maximum distance between points to be considered friends.
) {
    auto result = std::vector< std::vector<size_t> >(); // This will hold the final groups of friends.

    // Create a list of points, each with an index and a pointer to its coordinates in 'data'.
    std::vector<Point> unused;
    for(size_t i=0 ; i<npts ; ++i) {
        unused.push_back(std::make_pair(i, data + i*ndim));
    }

    // Continue forming groups until no points remain ungrouped.
    while( !unused.empty() ) {
        auto group = std::vector<size_t>(); // This will hold the current group of friends.
        auto toadd = std::vector<Point>();  // Points to be added to the group.
        toadd.push_back(unused.back());     // Start the group with the last point in 'unused'.
        unused.pop_back();                  // Remove that point from 'unused'.

        // Process all points to be added to the current group.
        while (!toadd.empty()) {
            auto point = toadd.back();
            toadd.pop_back();
            group.push_back(point.first); // Add the point's index to the group.

            // Check all unused points to see if they are within the linking length from the current point.
            for (auto& unused_point : unused) {
                if(dist(unused_point.second, point.second, ndim) < linking_length) {
                    toadd.push_back(unused_point); // Add to the list to be added to the group.
                    unused_point.second = nullptr; // Mark the unused point as processed.
                }     
            }

            // Remove points that have been added to a group from the 'unused' list.
            unused.erase(
                std::remove_if(unused.begin(), unused.end(), [](Point p) { return p.second == nullptr; }),
                unused.end()
            );
        }
        std::sort(group.begin(), group.end()); // Sort the group by indices for a consistent order.
        result.push_back(group); // Add the completed group to the result.
    }
    return result; // Return all groups found.
}
