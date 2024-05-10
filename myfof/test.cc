#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <vector>
#include <iostream>

namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

int main()
{
    // Define a point type and a box type
    typedef bg::model::point<float, 2, bg::cs::cartesian> point;
    typedef bg::model::box<point> box;

    // Define the type for R-tree using box and a dynamic quadratic node size
    bgi::rtree<box, bgi::dynamic_quadratic> rtree(bgi::dynamic_quadratic(16, 4)); // setting max_elems = 16 and min_elems = 4

    // Insert some boxes into the R-tree
    rtree.insert(box(point(0, 0), point(5, 5)));
    rtree.insert(box(point(5, 5), point(10, 10)));
    rtree.insert(box(point(2, 2), point(7, 7)));

    // Query the R-tree for boxes intersecting a given point
    point query_point(3, 3);
    std::vector<box> result;
    rtree.query(bgi::intersects(query_point), std::back_inserter(result));

    // Print the results
    std::cout << "Boxes containing point (3,3):" << std::endl;
    for (const auto& b : result) {
        std::cout << bg::dsv(b) << std::endl;
    }

    return 0;
}
