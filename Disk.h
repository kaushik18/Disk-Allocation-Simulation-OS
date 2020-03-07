// Kaushik Nadimpalli
// CS4348.001
// Project 3

// This header file contains the definitions of functions in Disk.cc
// It contain the declaration of the Disk class which is used in Disk.cc

class Disk
{
public:
	 Disk();
	 block read(int); //reading blocks
 	 void write(block, int); //writing blocks
	 
 private:
 	 block *d; //reference to block d, created in Disk.cc
};
