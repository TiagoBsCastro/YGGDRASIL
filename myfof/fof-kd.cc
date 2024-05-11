#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <memory>
#include <random>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<double, 3, bg::cs::cartesian> Point3D;
typedef std::pair<Point3D, int> Value;
typedef bg::model::box<Point3D> Box;
typedef bgi::rtree< Value, bgi::rstar<16> > RTree;

double pbc_distance(const Point3D& a, const Point3D& b, const double box_size[3]) {
    double dx = std::abs(bg::get<0>(a) - bg::get<0>(b));
    double dy = std::abs(bg::get<1>(a) - bg::get<1>(b));
    double dz = std::abs(bg::get<2>(a) - bg::get<2>(b));

    dx = std::min(dx, box_size[0] - dx);
    dy = std::min(dy, box_size[1] - dy);
    dz = std::min(dz, box_size[2] - dz);

    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

Box queryBox(const Point3D& point, double range, const double box_size[3]) {
    Point3D lower(bg::get<0>(point) - range, bg::get<1>(point) - range, bg::get<2>(point) - range);
    Point3D upper(bg::get<0>(point) + range, bg::get<1>(point) + range, bg::get<2>(point) + range);
    return Box(lower, upper);
}

void fof(RTree& tree, std::vector<Value>& points, double linking_length, const double box_size[3]) {
    int cluster_id = 0;
    std::vector<int> visited(points.size(), 0);

    for (size_t i = 0; i < points.size(); ++i) {
        if (visited[i] == 0) {
            std::vector<size_t> stack = {i};
            visited[i] = ++cluster_id;

            while (!stack.empty()) {
                size_t idx = stack.back();
                stack.pop_back();
                Point3D current_point = points[idx].first;
                Box search_box = queryBox(current_point, linking_length, box_size);

                // Perform a range query using the "within box" query
                std::vector<Value> neighbors;
                tree.query(bgi::within(search_box), std::back_inserter(neighbors));

                // Process each neighbor
                for (auto& neighbor : neighbors) {
                    double distance = pbc_distance(neighbor.first, current_point, box_size);
                    if (distance < linking_length) {
                        size_t neighbor_idx = neighbor.second;
                        if (visited[neighbor_idx] == 0) { // Check if not visited
                            visited[neighbor_idx] = cluster_id;  // Mark as visited
                            stack.push_back(neighbor_idx);
                        }
                    }
                }
            }
        }
    }

    // Update cluster IDs
    for (size_t i = 0; i < points.size(); ++i) {
        points[i].second = visited[i];
    }
}

int main() {
    double box_size[3] = {10.0, 10.0, 10.0}; // Simulation box dimensions
    std::vector<Value> points; // Points and their initial cluster IDs (0 initially)

    // Simulating points (this would be your data input section)
    for (int i = 0; i < 100000; ++i) {
        double x = (double)rand() / RAND_MAX * box_size[0];
        double y = (double)rand() / RAND_MAX * box_size[1];
        double z = (double)rand() / RAND_MAX * box_size[2];
        points.push_back(std::make_pair(Point3D(x, y, z), i));
    }

    RTree tree (points.begin(), points.end());

    double linking_length = 1.0;
    fof(tree, points, linking_length, box_size);

    return 0;
}
