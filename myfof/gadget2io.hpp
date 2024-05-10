/**
 * @file gadget2io.h
 * @brief Interfaces for handling Gadget-2 simulation data, including reading snapshot headers,
 * particle data, and performing unit conversions and other necessary preprocessing steps.
 */

#ifndef GADGET2IO_H
#define GADGET2IO_H

#include <cstdint>
#include <boost/geometry/geometry.hpp>
#include <fstream>
#include <string.h>

namespace bg = boost::geometry;

typedef std::size_t size_t;
typedef bg::model::point<double, 3, bg::cs::cartesian> point_t;
typedef std::pair<point_t, size_t> value_t;
typedef std::vector<std::pair<point_t, size_t>> points_t;

//#include "utilities.h"
//#include "data.h"

/**
 * @struct Header
 * @brief Represents the header information in a Gadget-2 simulation file.
 *
 * This structure stores various metadata about a Gadget-2 simulation, including particle counts,
 * mass array, time, redshift, and other simulation parameters.
 */
const int dummy = 14;
struct Header
{
  int32_t npart[6];
  double massarr[6];
  double time;
  double redshift;
  int32_t flag_sfr;
  int32_t flag_feedback;
  uint32_t npartTotal[6];
  int32_t flag_cooling;
  int32_t numfiles;
  double boxsize;
  double om0;
  double oml;
  double h;
  int32_t flag_sage;
  int32_t flag_metals;
  int32_t nTotalHW[6];
  int32_t flag_entropy;
  int32_t la[dummy];
};

/**
 * @brief Stream extraction operator for reading Header structure data from an input stream.
 * @param input Reference to the input stream from which to read.
 * @param header Reference to the Header structure to populate with read data.
 * @return Reference to the updated input stream.
 *
 * Reads binary data corresponding to the Header structure directly from the input stream into the provided Header object.
 */
inline std::istream &operator>>(std::istream &input, Header &header)
{
  input.read((char *)&header, sizeof(header));
  return input;
};

/**
 * @struct Block
 * @brief Stores block metadata in a Gadget-2 data file.
 *
 * This structure represents the metadata for a block in a Gadget-2 file, detailing its size,
 * alignment, and other necessary data for file parsing.
 */
struct Block
{
  int32_t blocksize1;
  int8_t alignment[4];
  char name[4];
  int8_t padding[8];
  int32_t blocksize2;
};

/**
 * @brief Stream extraction operator for reading Block structure data from an input stream.
 * @param input Reference to the input stream from which to read.
 * @param block Reference to the Block structure to populate with read data.
 * @return Reference to the updated input stream.
 *
 * Reads binary data corresponding to the Block structure directly from the input stream into the provided Block object.
 */
inline std::istream &operator>>(std::istream &input, Block &block)
{
  input.read((char *)&block, sizeof(block));
  return input;
};

/**
 * @brief Reads the header from a snapshot file and stores it in the provided Header structure.
 *
 * This function opens the specified input file stream, reads the header information into the
 * provided Header instance, and optionally closes the file stream.
 *
 * @param file_in Path to the input file containing the snapshot header.
 * @param header Reference to the Header structure where the header data will be stored.
 * @param fin Reference to the ifstream instance used for reading the file.
 * @param close Boolean flag indicating whether to close the ifstream after reading.
 * @return int Returns 0 on successful read, otherwise returns an error code.
 */
int readHeader(std::string file_in, Header &header, std::ifstream &fin, bool close);

/**
 * @brief Tests if the snapshot contains hydrodynamic (Hydro) particles.
 *
 * This function checks the Header of the snapshot to determine if hydrodynamic particles
 * are present based on the simulation parameters.
 *
 * @param p Reference to the InputParams structure containing simulation settings.
 * @param data Reference to the Header structure containing the snapshot header data.
 */
bool testHydro(Header &data);

/**
 * @brief Prints the details of a snapshot header.
 *
 * This function outputs the contents of the provided Header structure to the standard output,
 * typically used for debugging or information purposes.
 *
 * @param header The Header structure whose contents are to be printed.
 */
void printHeader(Header header);

/**
 * @brief Advances the file stream by skipping a specified number of variables.
 *
 * This function moves the read position of the given file stream forward by skipping over
 * a specified number of variables, each of a given size.
 *
 * @param fin Reference to the ifstream from which data is being read.
 * @param size The size of each variable to skip.
 * @param n The number of variables to skip.
 */
void fastforwardNVars(std::ifstream &fin, size_t size, size_t n);

/**
 * @brief Advances the file stream to the start of a specified block.
 *
 * This function searches for and advances the file stream to the beginning of a block
 * specified by its label. It also prints the file stream's metadata if the processor ID
 * (myid) is 0, typically used for debugging or logging.
 *
 * @param fin Reference to the ifstream from which data is being read.
 * @param BLOCK The label of the block to which the file stream is to be advanced.
 * @param myid Processor ID used to determine if metadata should be printed.
 */
void fastforwardToBlock(std::ifstream &fin, std::string BLOCK, int myid);

/**
 * @brief Reads the position data for the dark-matter component from a snapshot file stream and stores them in the specified vector.
 *
 * This function reads the position of each dark-matter particle from the snapshot file specified by `fin`.
 * The positions are normalized by the box size from the header and stored in the `xx` vector.
 * If `myid` is 0, it produces monitoring messages to aid in debugging or tracking the read progress.
 *
 * @param fin Reference to the input file stream from which the snapshot data is read. It is assumed that the file is already open.
 * @param data Reference to the `Header` structure that contains metadata about the snapshot, including the total number of particles and box size.
 * @param isnap Snapshot number (currently unused in the function but may be useful for logging or conditional processing).
 * @param xx Reference to a `points_t` structure (assumed to be a vector of structures or similar container) where each element's `first` member is a tuple or structure that can have its components set by the `set<>()` template method.
 * @param myid Identifier for the process or thread calling this function; if `myid` equals 0, additional monitoring output is generated.
 *
 * @note The function `fastforwardToBlock` is called to position the file stream at the beginning of the "POS " block.
 * @note The positions are scaled by dividing by the box size to normalize them according to the simulation dimensions.
 */
void readPos(std::ifstream &fin, Header &data, int isnap, points_t &xx, int myid);

#endif