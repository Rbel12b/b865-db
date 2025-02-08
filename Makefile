CXX = g++

INCLUDEPATH =

CFLAGS = -g -Wall -Wextra -fsanitize=address -fdiagnostics-color=always -std=c++17 $(INCLUDEPATH)
LDFLAGS = -fsanitize=address

ifeq ($(OS),Windows_NT)
	EXE_EXT = .exe
else
	EXE_EXT = 
endif

SRC = $(wildcard src/*.cpp) $(wildcard src/**/*.cpp)
OBJ = $(patsubst src/%.cpp,build/%.o,$(SRC))

APP = b865-db$(EXE_EXT)

.PHONY: all

all: build run 

run: $(APP)
	./$(APP) --help

build: $(APP)

$(APP): $(OBJ)
	$(CXX) -o $(APP) $(OBJ) $(LDFLAGS)

build/%.o: src/%.cpp .stamp
	$(CXX) $(CFLAGS) -o $@ -c $<

.stamp:
ifeq ($(OS),Windows_NT)
	echo > .stamp
else
	touch .stamp
endif
