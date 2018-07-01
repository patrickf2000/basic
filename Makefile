CXX=g++
FLAGS=-std=c++11
CFLAGS=$(FLAGS)
LDFLAGS=$(FLAGS)
BUILD_DIR=./build

SRC := $(wildcard ./src/*.cxx)
OBJECTS := $(patsubst ./src/%.cxx,$(BUILD_DIR)/%.o,$(SRC))

all: dirs objects

dirs:
	if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi
	
objects: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(BUILD_DIR)/basic $^
	
$(BUILD_DIR)/%.o: ./src/%.cxx
	$(CXX) $(CFLAGS) -c -o $@ $<
	
clean:
	rm -r ./build
	
install:
	install build/basic /usr/local/bin

