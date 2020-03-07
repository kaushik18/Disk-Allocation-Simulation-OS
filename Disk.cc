// Kaushik Nadimpalli
// CS4348.001
// Project 3

#include "project3.h"

// Disk is its own its own object with a set of public methods to use it. It serves as a storage device.
// The disk knows how to read and write blocks by block number.

Disk::Disk()
{
  d = new block[maxBlocks];
  // allocate new space on disk
  // set this to the maximum allowed
}

//Following function allows the disk to read the blocks
block Disk::read(int x) 
{
  return d[x];
}

//Following function allows the disk to write blocks bu blocknumber 
void Disk::write(block bl, int x) 
{
  if (bl.bytes.length() > maxSize)
  {
    std :: cerr << "ERROR: Block too large" << std :: endl;
	// Here we are checking the length of the blocks with the maximum block dize
	// We can give a warning message if the block is too big.
  }
  d[x] = bl;
}