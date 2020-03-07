// Kaushik Nadimpalli
// CS4348.001
// Project 3

// This file serves for user user input validation purpose.
// Purpose: A user interface should be written to interact with the disk object and to support the menu options shown below.  

#include "project3.h"

int userdataType(std::string userdata) //validates user input
{
  for (unsigned int x = 0; x < userdata.length(); ++x) 
  {
    userdata[x] = std::tolower(userdata[x]);
	//converts data into lowercase
	// below, we are making sure by ensuring this again
  }

  if (userdata == "contiguous" || userdata == "CONTIGUOUS")
  {
    return CONTIGUOUS; //allocates using contiguous method if user chooses this
  }
  else if (userdata == "chained" || userdata == "CHAINED")
  {
    return CHAINED; //allocates using chained method if user chooses this
  }
  else if (userdata == "indexed" || userdata == "INDEXED")
  { 
	return INDEXED; //allocates using indexed method if user chooses this
  }
  
  return -1;
}

std::string stringData() //character data validation check
{
  std::string value;
  std::getline(std::cin, value);
  return value;
}

int intData() //number data validation check
{
  std::string value;
  std::getline(std::cin, value);
  
  for (unsigned int x = 0; x < value.length(); ++x) 
  {
    if (!std::isdigit(value[x]))
      return -1;
  }
  return std::stoi(value);
}