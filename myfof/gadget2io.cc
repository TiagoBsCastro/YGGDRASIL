/* Routines to read Gadget2 snapshot format */
#include "gadget2io.hpp"

/* Reads the "file_in" snapshot and stores it on the "header" instance
   of Header. "fin" is a ifstream instance that is leaved open for further
   reading if "close" == false.
*/
int readHeader(std::string file_in, Header &header, std::ifstream &fin,
               bool close = false)
{

  /* Read the Snap Header*/
  fin.open(file_in.c_str());
  if (!fin)
  {
    fin.open((file_in.substr(0, file_in.size() - 2)).c_str());
  }
  if (!fin)
  {
    std::cerr << "Error in opening the file: " << file_in << "!\n\a";
    return 1;
  }

  int32_t blockheader[5];
  fin.read((char *)&blockheader, sizeof(blockheader));
  fin >> header;

  if (close)
    fin.close();
  return 0;
}

/* Tests if the snapshot has Hydro particles */
bool testHydro(Header &data)
{

  int dimmass0 = 0;
  for (int i = 0; i <= 5; i++)
  {
    if (data.massarr[i] == 0)
      dimmass0 += data.npart[i];
  }
  return bool(dimmass0);
  
}

/* Prints the Snapshot Header */
void printHeader(Header header)
{
  std::cout << "Printing Header Data" << std::endl;

  std::cout << "N. part.: " << header.npart[0] << " " << header.npart[1] << " " << header.npart[2] << " " << header.npart[3] << " " << header.npart[4] << " " << header.npart[5] << std::endl;
  std::cout << "Mass Array: " << header.massarr[0] << " " << header.massarr[1] << " " << header.massarr[2] << " " << header.massarr[3] << " " << header.massarr[4] << " " << header.massarr[5] << " " << std::endl;
  std::cout << "Time: " << header.time << std::endl;
  std::cout << "Z: " << header.redshift << std::endl;
  std::cout << "Flag SFR.: " << header.flag_sfr << std::endl;
  std::cout << "Flag Feedback: " << header.flag_feedback << std::endl;
  std::cout << "N. tot.: " << header.npartTotal[0] << " " << header.npartTotal[1] << " " << header.npartTotal[2] << " " << header.npartTotal[3] << " " << header.npartTotal[4] << " " << header.npartTotal[5] << " " << std::endl;
  std::cout << "Flag cooling: " << header.flag_cooling << std::endl;
  std::cout << "N. files: " << header.numfiles << std::endl;
  std::cout << "Box size: " << header.boxsize << std::endl;
  std::cout << "Omega_matter: " << header.om0 << std::endl;
  std::cout << "Omega_DE: " << header.oml << std::endl;
  std::cout << "h: " << header.h << std::endl;
  std::cout << "Flag sage: " << header.flag_sage << std::endl;
  std::cout << "Flag metals: " << header.flag_metals << std::endl;
  std::cout << "N. tot HW: " << header.nTotalHW[0] << " " << header.nTotalHW[1] << " " << header.nTotalHW[2] << " " << header.nTotalHW[3] << " " << header.nTotalHW[4] << " " << header.nTotalHW[5] << " " << std::endl;
  std::cout << "Flag entropy: " << header.flag_entropy << std::endl;
  std::cout << "  " << std::endl;
  std::cout << "      __________________ COSMOLOGY __________________  " << std::endl;
  std::cout << " " << std::endl;
  int dimmass0 = 0;

  for (int i = 0; i <= 5; i++)
  {
    if (header.massarr[i] == 0)
    {
      dimmass0 += header.npart[i];
    }
  }

  if (dimmass0 == 0)
  {

    std::cout << "		@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
    std::cout << "		@                          @" << std::endl;
    std::cout << "		@  !!DM only simulation!!  @" << std::endl;
    std::cout << "		@                          @" << std::endl;
    std::cout << "		@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
  }
  else
  {

    std::cout << "		@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
    std::cout << "		@                          @" << std::endl;
    std::cout << "		@  !!Hydro   simulation!!  @" << std::endl;
    std::cout << "		@                          @" << std::endl;
    std::cout << "		@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << std::endl;
  }

  std::cout << " .......................................................... " << std::endl;
  std::cout << "   number of particles in this snapshot: " << std::endl;
  std::cout << header.npart[0] << " " << header.npart[1] << " " << header.npart[2]
       << " " << header.npart[3] << " " << header.npart[4] << " " << header.npart[5] << std::endl;

  std::cout << "      Omegam = " << header.om0 << " "
       << "Omegal = " << header.oml << std::endl;
  std::cout << "           h = " << header.h << " "
       << "BoxSize = " << header.boxsize << std::endl;

  std::cout << "      _______________________________________________  " << std::endl;
  std::cout << " " << std::endl;
  std::cout << "   total number of particles in the simulation: " << std::endl;
  std::cout << header.nTotalHW[0] * pow(2, 32) + header.npartTotal[0] << " " << header.nTotalHW[1] * pow(2, 32) + header.npartTotal[1] << " " << header.nTotalHW[2] * pow(2, 32) + header.npartTotal[2] << " " << header.nTotalHW[3] * pow(2, 32) + header.npartTotal[3] << " " << header.nTotalHW[4] * pow(2, 32) + header.npartTotal[4] << " " << header.nTotalHW[5] * pow(2, 32) + header.npartTotal[5] << std::endl;
  std::cout << " " << std::endl;
  std::cout << "   xparticle type mass array: " << std::endl;
  std::cout << header.massarr[0] << " " << header.massarr[1] << " " << header.massarr[2]
       << " " << header.massarr[3] << " " << header.massarr[4] << " " << header.massarr[5] << std::endl;
}

/* Fastforward "n" variables of size "size" of the stream "fin". */
void fastforwardNVars(std::ifstream &fin, size_t size, size_t n)
{
  fin.seekg(n * size / sizeof(int8_t), fin.cur);
}

/* Fastforward the fstream "fin" to variable labled "BLOCK".
   Prints "fin" metadata if myid == 0.
*/
void fastforwardToBlock(std::ifstream &fin, std::string BLOCK, int myid)
{

  Block block;
  char nullchar = '\0';
  char blockStringLike[5];
  blockStringLike[4] = nullchar;
  bool iterate = true;

  while (iterate)
  {

    fin >> block;
    for (int i = 0; i < 4; i++)
      blockStringLike[i] = block.name[i];

    iterate = strcmp(blockStringLike, &BLOCK[0]);
    if (iterate)
    {
      if (myid == 0)
        std::cout << "Fast Fowarding next block. Name: " << blockStringLike << std::endl;
      fin.seekg(block.blocksize2, std::ios_base::cur);
    }
  }

  for (int i = 0; i < 4; i++)
    blockStringLike[i] = block.name[i];
  if (myid == 0)
  {
    std::cout << "reading next block. Name: " << blockStringLike << std::endl;
    std::cout << "Should be                 " << BLOCK << std::endl;
  }
}

/* Reads the position of dark-matter block of snapshot fin. The positions 
   are stored in the vector xx.

   Produces monitoration messages if myid == 0.
*/
void readPos(std::ifstream &fin, Header &data, int isnap, points_t &xx, int myid)
{

  float num_float1, num_float2, num_float3; // Dummy vars to read x,y, and z
  fastforwardToBlock(fin, "POS ", myid);
 
  /* Read DM only positions ptype=1 */
  {
    int i = 1;

    for (int pp = 0; pp < data.npart[i]; pp++)
    {

      float x, y, z;

      fin.read((char *)&num_float1, sizeof(num_float1));
      fin.read((char *)&num_float2, sizeof(num_float2));
      fin.read((char *)&num_float3, sizeof(num_float3));

      x = (((num_float1) / data.boxsize));
      y = (((num_float2) / data.boxsize));
      z = (((num_float3) / data.boxsize));

      
      xx[pp].first.set<0>( x );
      xx[pp].first.set<1>( y );
      xx[pp].first.set<2>( z );
      xx[pp].second = pp; // It should be the particle ID!
    }
  }
}