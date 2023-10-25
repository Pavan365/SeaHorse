STDS ?= -std=c++20
WFLAGS ?= -Wall -Wpedantic -Wno-deprecated-declarations -Wno-format-security -Wno-deprecated-enum-compare-conditional -Wno-deprecated-anon-enum-enum-conversion
DEFS ?= 
LIBS ?= -lseahorse -lraylib
IPATH ?=
LPATH ?= -L./libs/seahorse -L./libs/raylib
FRAMEWORKS ?= -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL


# Using libs...
IPATH += -I./libs
IPATH += -I./libs/eigen
IPATH += -I./include

# If using fftw3... This speeds up splitstep by a factor ~2
# We may or may not want this based on dimensions
DEFS += -DEIGEN_FFTW_DEFAULT
LIBS += -lfftw3 -lfftw3f -lfftw3l
LPATH += -L/usr/local/lib

# Release optimisation flags
OPTS += -mavx2 -mfma -march=native 
OPTS += -ffp-contract=fast -fno-math-errno 
# OPTS += -ffast-math -funsafe-math-optimizations # potentially wrong results - check?
OPTS := -Ofast
DEFS += -DNDEBUG

# Compilers
CC = gcc
CXX = g++

GREEN='\033[92m'
RED='\033[91m'
NC='\033[0m'

# Defaults to main.cpp in release mode
all: clean release

# Graphical Interface
gui: gui.cpp Makefile ./libs/seahorse/libseahorse.a
	@echo ${GREEN}[BUILDING]${NC} Graphical Version...
	$(CXX) $(FRAMEWORKS) $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) $(LPATH) $(LIBS) $@.cpp -o ./bin/$@
	$(RUN) ./bin/$@ $(ARGS)

# Main.cpp file
release: main.cpp Makefile ./libs/seahorse/libseahorse.a
	@echo ${GREEN}[BUILDING]${NC} Release Version...
	$(CXX) main.cpp $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) $(LPATH) $(LIBS) -o ./bin/$@
	$(RUN) ./bin/$@ $(ARGS)

# We don't depend on the lib as we just rebuild/destroy it anyway
debug: main.cpp Makefile
	@echo ${GREEN}[BUILDING]${NC} Debug Version... ${RED}[Optimisation flags must match libseahorse]${NC}
	- $(RUN) rm ./libs/seahorse/libseahorse.a
	$(CXX) libs/seahorse/src/seahorse.cpp $(STDS) $(DEFS) -O0 $(WFLAGS) $(IPATH) -c -g -o libs/seahorse/libseahorse.o
	$(RUN) ar rvs ./libs/seahorse/libseahorse.a ./libs/seahorse/libseahorse.o
	$(CXX) main.cpp $(STDS) $(DEFS) -O0 $(WFLAGS) $(IPATH) $(LPATH) $(LIBS) -g -o ./bin/$@
	- $(RUN) rm ./libs/seahorse/libseahorse.o
	- $(RUN) rm ./libs/seahorse/libseahorse.a
	$(RUN) ./bin/$@ $(ARGS)

# NB This must have EXACTLY the same architecture/optimisation flags set as the other builds
libs/seahorse/libseahorse.a : $(shell find ./libs/seahorse -type f ! -name libseahorse.a) Makefile
	@echo ${GREEN}[BUILDING]${NC} Lib Seahorse...
	$(CXX) libs/seahorse/src/seahorse.cpp $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) -c -o libs/seahorse/libseahorse.o
	$(RUN) ar rvs libs/seahorse/libseahorse.a libs/seahorse/libseahorse.o
	- $(RUN) rm libs/seahorse/libseahorse.o

libs/raylib/libraylib.a :
	

# Clean up any executables
clean:
	@echo ${GREEN}[CLEANING]${NC} ...
	rm -rf ./bin/*
	- rm libs/seahorse/libseahorse.a