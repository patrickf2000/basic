CXX=g++
FLAGS=-std=c++11
CFLAGS=$(FLAGS)
LDFLAGS=$(FLAGS)
BUILD_DIR=./build
DOC_DIR=./doc

SRC := $(wildcard ./src/*.cxx)
OBJECTS := $(patsubst ./src/%.cxx,$(BUILD_DIR)/%.o,$(SRC))

.PHONY: all clean doc
all: dirs objects

dirs:
	if [ ! -d $(BUILD_DIR) ]; then mkdir $(BUILD_DIR); fi
	
objects: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $(BUILD_DIR)/basic $^
	
$(BUILD_DIR)/%.o: ./src/%.cxx
	$(CXX) $(CFLAGS) -c -o $@ $<
	
doc:
	pdflatex doc/basic.tex --output-directory=${DOC_DIR}
	
clean:
	rm -r ./build
	rm *.{aux,log,toc,pdf}
	
install:
	install build/basic /usr/local/bin

