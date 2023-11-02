STDS ?= -std=c++20
WFLAGS ?= -Wall -Wpedantic -Wno-deprecated-declarations -Wno-format-security -Wno-deprecated-enum-compare-conditional -Wno-deprecated-anon-enum-enum-conversion
DEFS ?= 
LIBS ?= -lseahorse
IPATH ?=
LPATH ?= -L./libs/seahorse
FRAMEWORKS ?= -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL


# Using libs...
IPATH += -I./
IPATH += -I./libs/eigen
IPATH += -I./libs/raylib/src

# If using fftw3... This speeds up splitstep by a factor ~2
# We may or may not want this based on dimensions
DEFS += -DEIGEN_FFTW_DEFAULT
LIBS += -lfftw3 -lfftw3f -lfftw3l
LPATH += -L/usr/local/lib

# Release optimisation flags
OPTS := -Ofast
OPTS += -mavx2 -mfma -march=native 
OPTS += -ffp-contract=fast -fno-math-errno 
# OPTS += -ffast-math -funsafe-math-optimizations # potentially wrong results - check?
DEFS += -DNDEBUG

# Compilers
CC = gcc
CXX = g++

GREEN='\033[92m'
RED='\033[91m'
NC='\033[0m'

# Defaults to main.cpp in release mode
all: clean release

# Graphical Interface - (this is also inserted into the seahorse.app)
gui: gui.cpp Makefile ./libs/seahorse/libseahorse.a libs/raylib/src/libraylib.a
	@echo ${GREEN}[BUILDING]${NC} Graphical Version...
	$(CXX) $(FRAMEWORKS) $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) $(LPATH) -L./libs/raylib/src $(LIBS) -lraylib $@.cpp -o ./bin/$@
	@cp ./bin/gui ./seahorse.app/Contents/MacOS/seahorse
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
libs/seahorse/libseahorse.a : $(shell find ./libs/seahorse -type f ! -name libseahorse.a) Makefile libs/eigen libs/spectra
	@echo ${GREEN}[BUILDING]${NC} Lib Seahorse...
	$(CXX) libs/seahorse/src/seahorse.cpp $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) -c -o libs/seahorse/libseahorse.o
	$(RUN) ar rvs libs/seahorse/libseahorse.a libs/seahorse/libseahorse.o
	- $(RUN) rm libs/seahorse/libseahorse.o

# We have these checked out at a specific time so don't need to check for changes
libs/raylib/src/libraylib.a : libs/raylib/src/raylib.h libs/raygui/src/raygui.h
	@echo ${GREEN}[BUILDING]${NC} Lib Raylib...
	(cd libs/raylib/src && make PLATFORM=PLATFORM_DESKTOP RAYLIB_MODULE_RAYGUI=TRUE)

# Clean up any executables
clean:
	@echo ${GREEN}[CLEANING]${NC} ...
	- rm -rf ./bin/*
	- rm libs/seahorse/libseahorse.a
	- find libs/raylib/src/*.o | xargs rm