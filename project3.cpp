// Kaushik Nadimpalli
// CS4348.001
// Project 3

/* Project Description: This project will simulate disk allocation methods. It will allow the user to choose an allocation method 
   from among the three presented in the textbook.  
   It will allow the user to perform disk operations on a simulated disk. Storage for the disk should be kept in an array.  
   The disk should be its own object with a set of public methods to use it.  A disk only knows how to read and write blocks by block number.
   File names should be allowed up to at least 8 characters.  The maximum file size should be 10 blocks.  
*/

#include "project3.h"

void userChoices() // User interface inputs
	// will be displayed after user runs executable with desired allocation method
{
  std::cout << "1) Display a file\n"
	   	 	<< "2) Display the file table\n"
	   		<< "3) Display the free space bitmap\n"
	  		<< "4) Display a disk block\n"
	  		<< "5) Copy a file from the simulation to a file on the real system\n"
	        << "6) Copy a file from the real system to a file in the simulation\n"
	        << "7) Delete a file\n"
	        << "8) Exit"
		    << std::endl;
}

int main(int argc, char** argv) // command line arguments for user input
{
  if (argc != 2)  // Ensure that user input is correct and has two arguments like instructions specify. 
  {
    std::cerr << "ERROR: Invalid arguments. Enter in following format " << "./project3 [contiguous|indexed|chained]" << std::endl;
	std::cerr << "Example: ./project3 indexed" << std::endl;
    return 1;
  }

  int datatype = userdataType(std::string(argv[1]));
  std::cout << "type: " << datatype << std::endl;
 
  Disk disk;
  Driver driver(disk, datatype); 
  
  int input = 0;
  while (input != 8) //as long as user doesn't want to exit, we follow the switch case logic within while loop
  {
    userChoices();
    input = intData(); //user input entered by this point

    switch(input) 
   {
   
    case 1: //Display a file 
      {
		  std::cout << "Please enter the file name: ";
		  std::string fileName = stringData();
		  std::cout << driver.fileInfo(fileName) << std::endl;
      }
      break;
    
	case 2: //Display the file table
     	 std::cout << driver.blockInfo(ALLOCATEFILE).bytes << std::endl;
     	 break;
    
	case 3: //Display the free space bitmap
      {
		  block bl = driver.blockInfo(MAP);
		  for (int x = 0; x < maxBlocks / 32; ++x) 
		  {
	  		for (int y = 0; y < 32; ++y) 
			{
	   		 std::cout << bl.bytes[ y+(x*32) ];
		  	}
	  	  std::cout << std::endl;
		  }
      }
      break;
    
	case 4: //Display a disk block
      {
		  std::cout << "Please enter the block index: ";
		  int index = intData();
		  std::cout << driver.blockInfo(index).bytes << std::endl;
      }
      break;
    
	case 5: //Copy a file from the simulation to a file on the real system
      {
		  std::cout << "Please enter the file to write: ";
		  std::string input = stringData();
		  driver.realSystemFile(input);
      }
      break;
    
	case 6: //Copy a file from the real system to a file on the simulation
      {
		  std::string writefromFile, writetoFile;
		  std::cout << "Copy from: ";
		  writefromFile = stringData();
		  std::cout << "Copy to: ";
		  writetoFile = stringData();
		  driver.simulatedFile(writefromFile, writetoFile);
      }
      break;
    
	case 7: //Delete a file
      {
		  std::cout << "Please enter the file to remove: ";
		  std::string fileName = stringData();
		  driver.removeFile(fileName);
      }
      break;
   
    case 8: //Exit
      break;
   
    default: //any other output
      std::cerr << "ERROR: Invalid input. Choose a valid option." << std::endl;
   }
  }
}