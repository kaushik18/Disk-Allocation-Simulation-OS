// Kaushik Nadimpalli
// CS4348.001
// Project 3

#ifndef PROJECT3_H
#define PROJECT3_H

// Import all necessary libraries
// Some of these are exclusively used in the Driver class.
#include <iostream>
#include <iomanip>
#include <fstream>

#include <string>
#include <cstdlib>
#include <string.h>

#include <math.h>
#include <sstream>
#include <vector>
#include <algorithm>

struct block //This is our block structure we will be using. Each block contains 512 bytes. There are 256 blocks total in disk.
{
  std::string bytes;
  block() 
  {
    bytes = "";
  }
};

struct connect //Our connect structure will allow us to locate files and work with files - in simulation and real system.
{
  int first, last;
  connect() 
  {
    first = -1;
    last = -1;
  }
};

//include all necessary header files of the source files that contain functionality. This file is our main header file since main is in project3.cpp
#include "Disk.h"
#include "Driver.h"

bool openfstream(std::string, std::fstream&); //getting data from and writing data to a file, required imports
void userChoices();
int intData();

std::string stringData();
int userdataType(std::string);
enum enumVals {CONTIGUOUS, CHAINED, INDEXED, MAP, ALLOCATEFILE}; 
// we can make most the methods, butmap, and file allocation user-defined data types by creating an enumeration.

#define maxSize 512 //maximum bytes in each block
#define maxBlocks 256 //maximum number of blocks in disk
#endif 