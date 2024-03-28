# Makefile for Lab4
CC := nvcc
CFLAGS := -std=c++11
LDFLAGS := -lcudart
TARGET := lab4

all: $(TARGET)

$(TARGET): random_walk.cu
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
