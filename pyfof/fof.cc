#define BOOST_LOG_DYN_LINK 1 // Needed for logging
#include <iostream>
#include <vector>
#include <list>
#include <cmath>

#include <boost/geometry/geometry.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/for_each.hpp>

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/expressions.hpp>
#include <chrono>

#include "fof.hpp"
#include "fof_brute.hpp"

// Namespace aliases for easier access
namespace bg = boost::geometry;
namespace bmpl = boost::mpl;
namespace bgi = bg::index;
namespace bl = boost::log;

// Typedef for convenience
typedef std::size_t size_t;

void init_logging() {
    // Register a simple formatter factory to allow custom log formatting.
    bl::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");

    // Add a console log with a simple format that includes timestamp, severity, and the actual message.
    bl::add_console_log(
        std::cout,
        bl::keywords::format = "[%TimeStamp%] [%Severity%] %Message%"
    );

    // Add attributes like timestamp and process ID, which are often used in log formatting.
    bl::add_common_attributes();
}

void finalize_logging() {
    // Flush all sinks to make sure all logged messages are written out
    bl::core::get()->flush();

    // Remove all sinks
    bl::core::get()->remove_all_sinks();

}

/**
 * @brief Struct to set coordinates for a Boost Geometry point in a D-dimensional space.
 *
 * This template struct is designed to initialize and modify the coordinates of points
 * represented by Boost Geometry's point model. It provides a convenient way to set
 * the coordinates from a given array of doubles, abstracting the details of accessing
 * point dimensions directly.
 *
 * @tparam D The dimensionality of the space in which the point exists.
 */
template <size_t D>
struct point_setter {
    /// Type definition for a D-dimensional point using double precision.
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t;

    point_t& point; ///< Reference to the point object whose coordinates are to be set.
    double* loc;    ///< Pointer to an array of doubles containing the coordinates.

    /**
     * @brief Construct a new point setter object.
     *
     * @param point Reference to the point object to be modified.
     * @param loc Pointer to an array of doubles that holds the coordinates to set in the point.
     */
    point_setter(point_t& point, double* loc) : point(point), loc(loc) {}

    /**
     * @brief Function call operator that sets a specific coordinate of the point.
     *
     * This operator template allows setting a specific coordinate of the point,
     * identified by the index `i`. The index corresponds to the dimension of the
     * point whose coordinate will be set using the value from the `loc` array.
     *
     * @tparam U Type of the index, typically an integral type that can be a compile-time constant.
     * @param i The index of the dimension to set. This is zero-based and should be less than D.
     */
    template<typename U>
    void operator()(U i) {
        bg::set<i>(point, loc[i]);  // Set the i-th dimension of the point to loc[i]
    }
};

/**
 * @brief Struct to calculate the squared Euclidean distance between two D-dimensional points.
 *
 * This template struct is utilized to compute the squared distance between two points in a 
 * D-dimensional space. It leverages Boost Geometry to handle points in an n-dimensional Cartesian space.
 * The struct operates by iterating over each dimension of the points and accumulating the square of
 * the differences of their coordinates.
 *
 * @tparam D Dimensionality of the space in which the points exist.
 */
template <size_t D>
struct d2_calc {
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t; ///< Type alias for a point in D-dimensional space.

    const point_t &p1; ///< Reference to the first point.
    const point_t &p2; ///< Reference to the second point.
    double &d2;         ///< Reference to the double variable where the result (squared distance) will be stored.

    /**
     * @brief Construct a new d2_calc object.
     *
     * Initializes the structure with references to two points and a reference to a double to store the result.
     *
     * @param p1 Reference to the first point.
     * @param p2 Reference to the second point.
     * @param d2 Reference to a double to store the calculated squared distance.
     */
    d2_calc(const point_t &p1, const point_t &p2, double &d2) : p1(p1), p2(p2), d2(d2) {}

    /**
     * @brief Function call operator that calculates and accumulates the squared distance between the points for the i-th dimension.
     *
     * This operator template is invoked for each dimension of the points to calculate the square of the difference
     * of their coordinates in that dimension and accumulate it in 'd2'.
     *
     * @tparam U Typically an integer type representing the dimension index. Must be a compile-time constant.
     * @param i Index of the dimension for which to calculate the difference of the coordinates.
     */
    template<typename U>
    void operator()(U i) {
        d2 += pow(bg::get<i>(p1) - bg::get<i>(p2), 2); // Calculate square of the difference and accumulate
    }
};

/**
 * @brief Struct to add a scalar value to all coordinates of a D-dimensional point.
 *
 * This template struct provides functionality to modify a point by adding a scalar value to each of its coordinates.
 * It is designed to work with points in any specified number of dimensions (D) and uses Boost Geometry for handling
 * the point data. The operation is performed in-place, directly modifying the point provided to the constructor.
 *
 * @tparam D The dimensionality of the space in which the point exists.
 */
template <size_t D>
struct add_scalar_to_point {
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t; ///< Type alias for a D-dimensional Cartesian point using double precision.

    point_t &p; ///< Reference to the point object whose coordinates are to be modified.
    double c;   ///< Scalar value to be added to each coordinate of the point.

    /**
     * @brief Construct a new add_scalar_to_point object.
     *
     * Initializes the structure with a reference to a point and a scalar value. This setup allows the addition
     * of the scalar to each coordinate of the point when the function call operator is invoked.
     *
     * @param p Reference to the point whose coordinates will be modified.
     * @param c Scalar value to add to each coordinate of the point.
     */
    add_scalar_to_point(point_t &p, double c) : p(p), c(c) {}

    /**
     * @brief Function call operator that adds the scalar value to a specified coordinate of the point.
     *
     * This operator template allows adding the scalar value 'c' to a specific coordinate of the point,
     * identified by the index 'i'. The operation modifies the point in-place.
     *
     * @tparam U Typically an integer type representing the dimension index. Must be a compile-time constant.
     * @param i Index of the dimension to which the scalar value is to be added.
     */
    template<typename U>
    void operator()(U i) {
        double new_coord = bg::get<i>(p) + c; // Calculate new coordinate value
        bg::set<i>(p, new_coord); // Set the new coordinate value in the point
    }
};

// Main function to perform friends-of-friends clustering using an R-tree
template <size_t D>
std::vector<std::vector<size_t>> friends_of_friends_rtree(double *data, size_t npts, double linking_length) {
    
    typedef bg::model::point<double, D, bg::cs::cartesian> point_t;
    typedef std::pair<point_t, size_t> value_t;
    using tree_t = bgi::rtree<value_t, bgi::linear<16>>;
    typedef bmpl::range_c<size_t, 0, D> dim_range;

    init_logging();
    using namespace std::chrono;
    high_resolution_clock::time_point t1, t2;
    // Now BOOST_LOG_TRIVIAL will work as expected.
    BOOST_LOG_TRIVIAL(info) << "Starting to reserve space for points";
    t1 = high_resolution_clock::now();

    // Reserve space for points to avoid multiple reallocations
    std::vector<std::pair<point_t, size_t>> points;
    points.reserve(npts);
    t2 = high_resolution_clock::now();
    BOOST_LOG_TRIVIAL(info) << "Reserved space in " << duration_cast<milliseconds>(t2 - t1).count() << " ms";

    BOOST_LOG_TRIVIAL(info) << "Starting to populate R-tree with points";
    t1 = high_resolution_clock::now();
    // Populate the R-tree with the points from the data array
    for (size_t i = 0; i < npts; ++i) {
        point_t point;
        // Initialize point coordinates from the data array
        boost::mpl::for_each<dim_range>(point_setter<D>(point, data + i * D));
        points.push_back(std::make_pair(point, i)); // Pair point with its index
    }
    t2 = high_resolution_clock::now();
    BOOST_LOG_TRIVIAL(info) << "Populated R-tree in " << duration_cast<milliseconds>(t2 - t1).count() << " ms";

    BOOST_LOG_TRIVIAL(info) << "Creating an R-tree";
    t1 = high_resolution_clock::now();
    // Create an R-tree using the points vector
    tree_t tree(points.begin(), points.end());
    t2 = high_resolution_clock::now();
    BOOST_LOG_TRIVIAL(info) << "Created R-tree in " << duration_cast<milliseconds>(t2 - t1).count() << " ms";

    // This will store the groups of points that are within the linking length
    std::vector<std::vector<size_t>> groups;

    BOOST_LOG_TRIVIAL(info) << "Building groups";
    t1 = high_resolution_clock::now();
    // Loop until all points are grouped
    while (!tree.empty()) {
        std::vector<value_t> to_add;
        // Start with an arbitrary point from the R-tree
        to_add.push_back(*tree.qbegin(boost::geometry::index::satisfies([](value_t const &) { return true; })));
        tree.remove(to_add.begin(), to_add.end()); // Remove it from the tree to prevent re-processing

        // Process all points that need to be grouped
        for (auto to_add_i = size_t(0); to_add_i < to_add.size(); ++to_add_i) {
            std::vector<value_t> added;
            auto it = to_add.begin() + to_add_i;

            // Define a search box around the point, extending by the linking length in all dimensions
            point_t lower = it->first, upper = it->first;
            bmpl::for_each<dim_range>(add_scalar_to_point<D>(lower, -linking_length));
            bmpl::for_each<dim_range>(add_scalar_to_point<D>(upper, linking_length));
            bg::model::box<point_t> box(lower, upper);

            // Define a predicate to determine if points are within the linking length
            auto within_ball = [&it, linking_length](value_t const &v) {
                double d2 = 0.;
                bmpl::for_each<dim_range>(d2_calc<D>(it->first, v.first, d2));
                return sqrt(d2) < linking_length;
            };

            // Query the R-tree for points within the box and within the linking length
            tree.query(bgi::within(box) && bgi::satisfies(within_ball), std::back_inserter(added));

            // Add newly found points to the group
            for (auto p : added) {
                to_add.push_back(p);
            }

            // Remove these points from the tree to avoid reprocessing
            tree.remove(added.begin(), added.end());

            if (tree.empty()) {
                break; // Exit if all points have been grouped
            }
        }

        // Compile the list of indices for the points in the current group
        std::vector<size_t> group;
        for (auto p : to_add) {
            group.push_back(p.second);
        }
        groups.push_back(group); // Add the current group to the list of all groups
    }

    t2 = high_resolution_clock::now();
    BOOST_LOG_TRIVIAL(info) << "Groups built in " << duration_cast<milliseconds>(t2 - t1).count() << " ms";
    BOOST_LOG_TRIVIAL(info) << "Completed friends-of-friends grouping";
    // Finalize logging before exiting the program
    finalize_logging();

    return groups; // Return all the groups found
}

// General interface function to handle different dimensions
std::vector<std::vector<size_t>> friends_of_friends(double *data, size_t npts, size_t ndim, double linking_length) {
    switch (ndim) {
        case 1: return friends_of_friends_rtree<1>(data, npts, linking_length);
        case 2: return friends_of_friends_rtree<2>(data, npts, linking_length);
        case 3: return friends_of_friends_rtree<3>(data, npts, linking_length);
        case 4: return friends_of_friends_rtree<4>(data, npts, linking_length);
        default: return friends_of_friends_brute(data, npts, ndim, linking_length);
    }
}
