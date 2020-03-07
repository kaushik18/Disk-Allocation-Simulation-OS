// Kaushik Nadimpalli
// CS4348.001
// Project 3

// This is a header file for the driver.
// The driver contains a lot of functions which are crucial for this program
// It also takes care of the three allocation methods, and the list of these functions is below.

class Driver 
{
 private:
  Disk disk;
  int allocType;
  std::vector<int> margin;

  void increment(std::string, int, int);
  void allocateData(std::string, int&, int&);
  void removeChainedIndexedBlocks(int);
  void writeRanBlock(block);
  void removeBlock(int);
  void alterMap(int);
 
  int retrieveBlock(int);
  int fileSize(const char*);
  int addSpace();
 
  connect releaseContiguous(int);
  connect retrieveFile(std::string);

 public:
  Driver(Disk&, int);
  block blockInfo(int);
  std::string fileInfo(std::string);
  
  void writeToBlock(block, int);
  void addFile(std::string);
  void removeFile(std::string);
  void simulatedFile(std::string, std::string);
  void realSystemFile(std::string);
};