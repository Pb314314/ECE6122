# Compiler and compiler flags
CC = /opt/homebrew/bin/g++-13
CFLAGS = -Wall -O2 -std=c++11 -fopenmp  # Add -fopenmp for OpenMP support

# Source files and executable name
SRC = main.cpp ECE_ElectricField.cpp ECE_PointCharge.cpp
EXECUTABLE = Lab2

# Compile the program
$(EXECUTABLE): $(SRC)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SRC)

# Clean up generated files
clean:
	rm -f $(EXECUTABLE)
