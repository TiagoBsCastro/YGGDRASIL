#include <iostream>
#include <vector>
#include <random>
#include <boost/geometry.hpp>

#include "fof.hpp"
#include "fof_brute.hpp"

namespace bg=boost::geometry;
typedef bg::model::point<double, 3, bg::cs::cartesian> Point3D;
typedef std::pair<Point3D, int> Value;

int main() {
    // Define some test data points in 3D space
    double box_size[3] = {10.0, 10.0, 10.0}; // Simulation box dimensions
    std::vector<double> points; // Points and their initial cluster IDs (0 initially)

    // Simulating points (this would be your data input section)
    for (int i = 0; i < 100000; ++i) {
        double x = (double)rand() / RAND_MAX * box_size[0];
        double y = (double)rand() / RAND_MAX * box_size[1];
        double z = (double)rand() / RAND_MAX * box_size[2];
        points.push_back(x);
        points.push_back(y);
        points.push_back(z);
    }
    double linking_length = 1.0;  // Linking length for clustering

    // Run the friends-of-friends algorithm
    std::vector<std::vector<size_t>> clusters = friends_of_friends(&points[0], points.size()/3, 3, linking_length);

    // Print the resulting clusters
    std::cout << "Clusters formed:" << clusters.size() << std::endl;

    return 0;
}