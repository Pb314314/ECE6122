# Makefile


CXX = g++


CXXFLAGS = -std=c++11


TARGET = Lab1


SRCS = main.cpp ECE_ElectricField.cpp ECE_PointCharge.cpp threadpool.cpp


all: $(TARGET)


$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $^ -o $@


clean:
	rm -f $(TARGET)