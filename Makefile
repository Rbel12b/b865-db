CXX = g++

INCLUDEPATH =

CFLAGS = -g -O3 -fdiagnostics-color=always -std=c++17 $(INCLUDEPATH)
CFLIBFLAGS = $(CFLAGS)

ifeq ($(OS),Windows_NT)
	LDFLAGS := 
	EXE_EXT = .exe
else
	LDFLAGS := 
	EXE_EXT = 
endif

SRC = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

APP = b865-db$(EXE_EXT)

.PHONY: all

all: build run 

run: $(APP)
	./$(APP) build/test.cdb

build: $(APP)

$(APP): $(LIBOBJ) $(OBJ)
	$(CXX) -o $(APP) $(OBJ) $(LIBOBJ) $(LDFLAGS)

build/%.o: src/%.cpp .stamp
	$(CXX) $(CFLAGS) -o $@ -c $<

lib/%.o: lib/%.cpp $(wildcard lib/**/.h)
	$(CXX) $(CFLIBFLAGS) -o $@ -c $<

lib/%.o: lib/%.cc $(wildcard lib/**/.h)
	$(CXX) $(CFLIBFLAGS) -o $@ -c $<

.stamp:
ifeq ($(OS),Windows_NT)
	echo > .stamp
else
	touch .stamp
endif