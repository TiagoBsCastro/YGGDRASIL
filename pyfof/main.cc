#include <iostream>
#include <vector>

#include "fof.hpp"
#include "fof_brute.hpp"

int main() {
    // Define some test data points in 3D space
    double data[] = {
        0.0, 0.0, 0.0,  // Point 0
        1.0, 1.0, 1.0,  // Point 1
        2.0, 2.0, 2.0,  // Point 2
        5.0, 5.0, 5.0,  // Point 3 - This should be a separate cluster if linking length is small
        0.5, 0.0, 0.0   // Point 4 - Close to Point 0
    };
    size_t npts = 5;  // Number of points
    double linking_length = 1.5;  // Linking length for clustering

    // Run the friends-of-friends algorithm
    std::vector<std::vector<size_t>> clusters = friends_of_friends(data, npts, 3, linking_length);

    // Print the resulting clusters
    std::cout << "Clusters formed:" << std::endl;
    for (const auto& cluster : clusters) {
        std::cout << "Cluster: ";
        for (size_t idx : cluster) {
            std::cout << idx << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}