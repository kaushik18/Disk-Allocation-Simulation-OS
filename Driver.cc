// Kaushik Nadimpalli
// CS4348.001
// Project 3

/* 
Purpose: The simulation should allow the user to choose the allocation method to use when the simulation begins, either through 
a menu or as a command line argument.  For the chained and indexed allocations, a free block should be randomly chosen.  For a contiguous 
allocation, choose the first contiguous set of blocks large enough for the request.
*/

#include "project3.h"

Driver::Driver(Disk& disk, int allocType) 
	// Return error is the three allocated methods are not chosen.
	// We are only working with these three allocation methods for this program.
{
  if (allocType != CONTIGUOUS && allocType != CHAINED && allocType != INDEXED) 
  {
    std::cerr << "ERROR: Invalid allocation method Type" << std::endl;
    exit(1); // termination upon failure
  }
  
  this->disk = disk; //local variable name is same as our member name
  this->allocType = allocType; //local variable name is same as our member name

  block bl;
  bl.bytes = std::string(maxBlocks, '0');
  bl.bytes[0] = '1';
  bl.bytes[1] = '1';
  disk.write(bl, MAP); //disk writes block

  // There will be free space generated and the block will go to a randomly shuffled place
  // if the allocation methos chosen is chained or indexed.
  if (allocType == CHAINED || allocType == INDEXED) 
  {
    for (int x = 2; x < maxBlocks; ++x) 
	{
      margin.push_back(x);
    }
    std::random_shuffle(margin.begin(), margin.end());
	//random shuffle is a method that randomly chooses the starting and ending areas
  }
}

// The purpose of this function is to add to File table
void Driver::increment(std::string file_name, int start, int length) 
{
  block b = blockInfo(ALLOCATEFILE);
  b.bytes += file_name + "\t" + std::to_string(start) + "\t" + std::to_string(length) + "\n";
  writeToBlock(b, ALLOCATEFILE); 
}

block Driver::blockInfo(int x) //We are reading a block and pointing to the next block to read
{
  if (x >= 0 && x < maxBlocks)//as long as max blocks is not crossed and 0 or more blocks exist
   {	
    return disk.read(x); //read disk
   }
  else
  {
    std::cerr << "ERROR: Our of range index" << std::endl;
  }
  return *(new block); //points to the new block
}

void Driver::alterMap(int index) //we alter the map whenever we are removing or writing to a block.
{
  block b = blockInfo(MAP);
  if (b.bytes[index] == '0')
  {
    b.bytes[index] = '1';
  }
  else
  {
    b.bytes[index] = '0';
  }
  disk.write(b, MAP); 
}

void Driver::removeBlock(int x)  // In this function we are removing a block
{
  disk.write(*(new block), x);
  if (blockInfo(x).bytes[x] != '0') //block is not empty
  {
    alterMap(x); //changes the map after we remove a block
  }
}

void Driver::writeToBlock(block b, int i) // Here we are writing data to a block
{
  disk.write(b, i);
  if (blockInfo(MAP).bytes[i] == '0') 
  {
    alterMap(i);
  }
}

// Here we are incrementing the number returned from allocation methods.
// We will be using this function later on and will be explained there.
int retrieveNums(int number) 
{
  int nums = 0;
  while (number) 
  {
    number /= 10; //divide number by 10
    nums++; //increment 
  }
  return nums;
}

int Driver::retrieveBlock(int fileSize) //get the block
{
  return ceil((double)fileSize / maxSize);
  // ceil willl round upwards the result
}

// Purpose of this function is to remove file
// Of course it will be done a little differently depending on
// the type of allocation method used. 
void Driver::removeFile(std::string file_name)
{
  block bl = blockInfo(ALLOCATEFILE);
  if (bl.bytes == "") //no files exist at the moment
  {
    std::cerr << "ERROR: There are no files to delete" << std::endl;
    return;
  }
  connect pl = retrieveFile(file_name);
  if (pl.first == -1) //file not found
  {
    std::cout << "ERROR: File to delete was not found" << std::endl;
    return;
  }

  std::string file_info = bl.bytes.substr(pl.first, pl.last);
  int start, file_size;
  allocateData(file_info, start, file_size);
  int blocks = retrieveBlock(file_size);

  // Contiguous removal implementation
  if (allocType == CONTIGUOUS) 
  {
    for (int x = start; x < start + blocks; ++x)
	{	
      removeBlock(x);
    }
  }
  
  //Chained removal implementation
  else if (allocType == CHAINED)
  {
    int ptSize = retrieveNums(maxBlocks);
    int address = start;
    do 
	{
      //We have to first read the starting pointer and so forth.
      int temp = std::stoi(blockInfo(address).bytes.substr(0, ptSize));
      removeChainedIndexedBlocks(address);
      address = temp;
    } while (address != 0);    
  } 
  
  //Indexed removal implementation
  else 
  {  
	//  indexList is a list of indices held by the index file
    //  l is a line to read from index file
    //  str is the index file stringstream
    std::vector<int> indexList;
    std::string l;
    std::stringstream str(blockInfo(start).bytes);
    
    // Here we are retreiving all the addresses from the input file
    while (getline(str,l))
      indexList.push_back(std::stoi(l));
    
    // Here we are reading each of the address in the input file
    for (int x = 0; x < (int)indexList.size(); ++x)
	{
      removeChainedIndexedBlocks(indexList[x]);
    }
    removeChainedIndexedBlocks(start);
  }
  
  bl.bytes.erase(pl.first, file_info.length());
  writeToBlock(bl, ALLOCATEFILE);
}


int Driver::fileSize(const char* file_name) 
{
  std::ifstream file(file_name, std::ifstream::in | std::ifstream::ate);
  // here we are making sure that the output position starts at end of file or 
  // at that the file is open for reading.
  if (!file.is_open())
    return -1;
  
  // Sets the position of the next character to be extracted from the input stream.
  // beg refers to beginning of the stream and end refers to the end of the stream.
  file.seekg(0, std::ios::end);
  int file_size = file.tellg();
  file.seekg(0, std::ios::beg);
  file.close(); //close file
  
  return file_size;
}

connect Driver::releaseContiguous(int fileSize) 
{
  connect contiguous;
  contiguous.first = -1;
  contiguous.last = -1;
  
  //Here we must iterate through the bit map and look for available space
  int allocSize = fileSize, x;
  for (x = 2; x < maxSize && allocSize != 0; ++x)
   {
    //bit - start of an open allotment
    if (blockInfo(1).bytes[x] == '0' && contiguous.first == -1)
	{
      contiguous.first = x;
    }
	//bit - signals that more open space is left
    if (blockInfo(1).bytes[x] == '0')
	{	
      allocSize--;
    }
    // Contiguguous chain has not enough space
    else 
	{
      contiguous.first = -1;
      allocSize = fileSize;
    }
    //if current contiguous chain has enough space
  }
  contiguous.last = x - 1;

  //pass arr if not enough
  if (allocSize != 0) 
  {
    contiguous.first = -1;
    contiguous.last = -1;
  }
  return contiguous;
}

void Driver::removeChainedIndexedBlocks(int x)
	//function to remove chained and indexed blocks
{
  margin.push_back(x);
  removeBlock(x);
}

connect Driver::retrieveFile(std::string file_name) 
	// In this function we are retrieving the file
	// in system or simulation
{
  connect x;
  x.first = -1;
  x.last = -1;
  std::string strt;
  std::stringstream str(blockInfo(ALLOCATEFILE).bytes);
  int areaUsed = 0;
  while (getline(str, strt)) 
  {
    if (strt.substr(0, strt.find("\t")) == file_name)
	{
      x.first = areaUsed;
      x.last = areaUsed + strt.length() + 1;
      break;
    }
    areaUsed += (strt.length() + 1); 
	// this is the length of the newline and the current string.
  } 
  return x;
}

void Driver::allocateData(std::string file_info, int& start, int& length) 
	// In this function we are splitting the string that contains the data
	// of the file into three different strings, and finding the beginning and 
	// the length of them.
{
  char *split;
  split = strtok((char*)file_info.c_str(),"\t");
  split = strtok(NULL,"\t");
  start = stoi(std::string(split));
  split = strtok(NULL,"\n");
  length = stoi(std::string(split));
  
  // strtok will split the tokens.
  // sequence of calls to this function split str into tokens, which are sequences of contiguous 
  // characters separated by any of the characters that are part of delimiters.
}

std::string Driver::fileInfo(std::string file_name) 
	// This function contains the pivotal data of the 
	// files we will be working with
{
  std::string filedata = "";
  int start;
  int area;
  connect x = retrieveFile(file_name);
  if (x.first != -1) 
  {
    std::string file_info = blockInfo(ALLOCATEFILE).bytes.substr(x.first,x.last);
    allocateData(file_info, start, area);
    int blocks = retrieveBlock(area);

    //if contiguous allocation allocType
    if (allocType == CONTIGUOUS) 
	{
      for (int x = start; x < start + blocks; ++x) 
	  {
		  filedata += blockInfo(x).bytes;
      }
    }
    //Chained allocation allocType
    else if (allocType == CHAINED) 
	{
      int ptSize = retrieveNums(maxBlocks);
      int address = start;
      do 
	  {
		  // This reads the first pointer
		  block bl = blockInfo(address);
		  filedata += bl.bytes.substr(ptSize, bl.bytes.length());
		  address = std::stoi(blockInfo(address).bytes.substr(0, ptSize));
      } while (address != 0);
    }

    //Indexed allocation allocType
    else 
	{
	//  indexList is a list of indices held by the index file
    //  l is a line to read from index file
    //  str is the index file stringstream
      std::vector<int> indexList;
      std::string l;
      std::stringstream ss(blockInfo(start).bytes);

      //Get every address from the input file
      while (getline(ss,l))
	indexList.push_back(std::stoi(l));

      //Read each of the address inside the input file
      for (int x = 0; x < (int)indexList.size(); ++x)
		  filedata += blockInfo(indexList[x]).bytes;
    }
  }
  return filedata;
}

void Driver::realSystemFile(std::string file_name) 
	// This function serves to work with the file on real system
{
  std::string filedata = fileInfo(file_name);
  if (filedata != "") 
  {
    std::fstream file(file_name, std::ios::out);
    file << filedata;
  }
  else
  {
    std::cerr << "ERROR: File " << file_name << " is either empty or does not exist." << std::endl;
	// file in real system is empty or does not exist
  }
}

void Driver::writeRanBlock(block bl)
	// This function serves for the disk to write to a random block
{
  int index = margin.front();
  margin.erase(margin.begin());
  writeToBlock(bl, index);
}

int Driver::addSpace() 
	//This function adds open space to the map
	//Occurs when we are removing files or blocks
{
  if (allocType == INDEXED || allocType == CHAINED) 
  {
    return margin.size();
  }
  int openspace = 0;
  std::string map = blockInfo(MAP).bytes;
  for (unsigned int x = 0; x < map.length(); ++x) 
  {
    if (map[x] == '0')
      openspace++;
  }
  return openspace;
}

void Driver::simulatedFile(std::string writefromFile, std::string writetoFile) 
	// This function is for the file in the simulation
{
  if (writefromFile == "" && writetoFile == "") 
  {
    std::cerr << "ERROR: Please provide a name for file" << std::endl;
    return;
  }
  int file_size = fileSize(writefromFile.c_str()), blocks = retrieveBlock(file_size);
  std::fstream file(writefromFile, std::ifstream::in);

  if (!file.is_open()) 
  {
    std::cerr << "ERROR: The file \"" <<  writefromFile << "\" failed to open." << std::endl;
    return;
  }

  
   // Contiguous allocation method
   
  if (allocType == CONTIGUOUS) 
  {
    if (blocks > 10 || blocks <= 0) 
	{
      std::cerr << "ERROR: File is empty or too large(10 blocks max)" << std::endl;
      return;
    }
    //contiguous = point in the free space bitmap which the file can be allocated
    connect contiguous = releaseContiguous(blocks);
    
    //If there was no contiguous space available
    if (contiguous.first < 0) 
	{
      std::cerr << "ERROR: Not enough contiguous space available" << std::endl;
      return;
    } 
    int spaceToAllocate = file_size;
    for (int x = contiguous.first; x <= contiguous.last; ++x) 
	{
		// Retrieve remaining space
		// This space will be used for maxsized blocks
      int space = (spaceToAllocate > maxSize) ? maxSize: spaceToAllocate;   
      spaceToAllocate -= maxSize;
      block bl;
      char* ch = new char[space];
      file.read(ch, space);
      bl.bytes = std::string(ch);
      writeToBlock(bl, x);
    }
    increment(writetoFile, contiguous.first, file_size);
  }

  
   // Chained allocation method
  
  else if (allocType == CHAINED) 
  {
    // ptSize is size to add pointer to the file, spaceToAllocate = how much of the file is left to read
    // start = where the first block will be placed, fileSizePts is file size when pointers are added
    // x represents the position the next byte will be read
     
    const int ptSize = retrieveNums(maxBlocks);
    int spaceToAllocate = file_size, start = margin.front(), fileSizePts = file_size + (blocks * ptSize);
    // If pointer is pointing to a location, we retrieve the new blocksize.

    blocks = retrieveBlock(fileSizePts);

    // Low freespace available for allocation
    if ( addSpace() < blocks ) 
	{
      std::cerr << "ERROR: There is not enough space available" << std::endl;
      return;
    }

    // The block is very large, not suitable so prompt user to adjust or use different file.
    if ( blocks <= 0 || blocks > 10 ) 
	{
      std::cerr << "ERROR: The File too large(pointer size could cause large blocks). Please adjust file size and try again." << std::endl;
      return;
    }

    char* ch = new char[file_size];
    file.read(ch,file_size);
    std::string str = std::string(ch);

    // Here we are putting all the blocks in the disk
    for (int x = 0; x < blocks; ++x) 
	{
      //256 size blocks allocation - get that space
      int space = (spaceToAllocate > maxSize - ptSize) ? (maxSize - ptSize) : spaceToAllocate;
      // allocated space required
      spaceToAllocate -= (maxSize - ptSize);

      block bl;

      // Pointer written at start of file
      if (x == blocks - 1) 
	  {
		  std::stringstream str;
		  str << std::setfill('0') << std::setw(ptSize) << "";
		  bl.bytes = str.str();
      }
      // If space not permitted, then write at next address
      else 
	  {
		  std::stringstream str;
		  str << std::setfill('0') << std::setw(ptSize) << margin.at(1);
		  bl.bytes = str.str();
      }
      
      bl.bytes += str.substr(x, space);
      x += space;     
      writeRanBlock(bl);
    }
	
    increment(writetoFile, start, fileSizePts);
  }

  // Indexed allocation method

  else if (allocType == INDEXED) 
  {
     // fileIndexSize is addition of index file size and the file size
     // spaceToAllocate is the space in the file that is still left to be read
     
    int fileIndexSize = file_size + maxSize, spaceToAllocate = file_size;
    std::vector<int> indexList;
    for (int x = 0; x < blocks; ++x) 
	{
      // Retrieve leftover space
	  // Will be used to get the maximum sized blocks
      int area = (spaceToAllocate > maxSize) ? maxSize: spaceToAllocate;
      spaceToAllocate -= maxSize;
      block bl;
      
	  char* ch = new char[area];
      file.read(ch, area);
      bl.bytes = std::string(ch);
      std::cout << bl.bytes << std::endl;
      
	  indexList.push_back(margin.front());
      writeRanBlock(bl);
    }

    block newfileIndex;
    for (int x = 0; x < (int)indexList.size(); ++x) 
	{
      newfileIndex.bytes += std::to_string(indexList[x]) + "\n";
    }
    
	int start = margin.front();
    writeRanBlock(newfileIndex);
    increment(writetoFile, start, fileIndexSize);
  }  
  file.close();
}