#include <iostream>
#include <vector>

#include "fof.hpp"

int main() {
    
    double linking_length = 0.01;  // Linking length for clustering

    // Run the friends-of-friends algorithm
    std::vector<std::vector<size_t>> clusters = friends_of_friends_rtree("/home/tcastro/Pinocchio/example/snap_000", linking_length);

    // Print the resulting clusters
    std::cout << "Clusters formed: " << clusters.size() << std::endl;

    return 0;
}