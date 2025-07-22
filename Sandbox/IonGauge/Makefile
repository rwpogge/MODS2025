#Author: Xander Carroll

#This Makefile compiles all *.c files in SRC_DIR into object files in BIN_DIR. 
#Then it combines all the object files into an executable.

#The name of the target executable.
TARGET=test

#The directory where the source code is, and where the object files should be created.
SRC_DIR=./src
BIN_DIR=./bin

#The compiler that should be used.
CXX=gcc

#The flags that should be used with the gcc command.
C_FLAGS=-Wall

#A list of all the .c files in the SRC_DIR directory, and an altered list of those files with a .o extension.
C_FILES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(C_FILES) )

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(C_FLAGS) $(OBJECTS) -o $(TARGET)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BIN_DIR)
	$(CXX) $(C_FLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(TARGET)