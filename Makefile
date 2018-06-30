CXX=g++
FLAGS=-std=c++11
CFLAGS=-c $(FLAGS) -o
LD_FLAGS=$(FLAGS) -o
BUILD_DIR=./build

OBJECTS=$(BUILD_DIR)/io.o \
	$(BUILD_DIR)/str.o \
	$(BUILD_DIR)/file.o \
	$(BUILD_DIR)/list.o \
	$(BUILD_DIR)/loop.o \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/math.o \
	$(BUILD_DIR)/vars.o \
	$(BUILD_DIR)/interpreter.o

all: dirs objects

dirs:
	if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi
	
objects: $(OBJECTS)
	$(CXX) $(OBJECTS) $(LD_FLAGS) $(BUILD_DIR)/basic
	
$(BUILD_DIR)/io.o: src/io.cxx
	$(CXX) src/io.cxx $(CFLAGS) $(BUILD_DIR)/io.o
	
$(BUILD_DIR)/str.o: src/str.cxx
	$(CXX) src/str.cxx $(CFLAGS) $(BUILD_DIR)/str.o
	
$(BUILD_DIR)/file.o: src/file.cxx
	$(CXX) src/file.cxx $(CFLAGS) $(BUILD_DIR)/file.o
	
$(BUILD_DIR)/list.o: src/list.cxx
	$(CXX) src/list.cxx $(CFLAGS) $(BUILD_DIR)/list.o
	
$(BUILD_DIR)/loop.o: src/loop.cxx
	$(CXX) src/loop.cxx $(CFLAGS) $(BUILD_DIR)/loop.o
	
$(BUILD_DIR)/main.o: src/main.cxx
	$(CXX) src/main.cxx $(CFLAGS) $(BUILD_DIR)/main.o
	
$(BUILD_DIR)/math.o: src/math.cxx
	$(CXX) src/math.cxx $(CFLAGS) $(BUILD_DIR)/math.o
	
$(BUILD_DIR)/vars.o: src/vars.cxx
	$(CXX) src/vars.cxx $(CFLAGS) $(BUILD_DIR)/vars.o
	
$(BUILD_DIR)/interpreter.o: src/interpreter.cxx
	$(CXX) src/interpreter.cxx $(CFLAGS) $(BUILD_DIR)/interpreter.o
	
clean:
	rm -r ./build
	
install:
	install build/basic /usr/local/bin

