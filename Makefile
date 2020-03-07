# Kaushik Nadimpalli
# CS4348.001
# Project 3

#Makefile used for compilation and production of the project executable.
#Executable will be used to run the simulation for the program.

PROJECT_NAME=project3
EXEC=$(PROJECT_NAME)
CXX=g++
CPP=g++
CXXFLAGS=-std=c++11 -Wall -pedantic -g
OBJS=$(PROJECT_NAME).o userInput.o Disk.o Driver.o
HEADER=Disk.h Driver.h $(PROJECT_NAME).h

all: $(EXEC)

backup: clean
	@mkdir -p ~/backups; chmod 700 ~/backups
	@$(eval CURDIRNAME := $(shell basename `pwd`))
	@$(eval MKBKUPNAME := ~/backups/$(PROJECT_NAME)-$(shell date +'%Y.%m.%d-%H:%M:%S').tar.gz)
	@echo
	@echo Writing backup file to: $(MKBKUPNAME)
	@echo
	@-tar zcfv $(MKBKUPNAME) ../$(CURDIRNAME)
	@echo
	@chmod 600 $(MKBKUPNAME)
	@echo
	@echo Done!

tarball: clean
	@$(eval CURDIRNAME := $(shell basename `pwd`))
	@-tar zcfv ../$(PROJECT_NAME).tar.gz ../$(CURDIRNAME)
	@echo
	@echo Done!

clean:
	rm -f *~ *.o *\# $(PROJECT_NAME)

$(EXEC): $(OBJS) $(HEADER)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)