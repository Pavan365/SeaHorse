STDS ?= -std=c++20
WFLAGS ?= -Wall -Wpedantic -Wno-deprecated-declarations -Wno-format-security -Wno-deprecated-enum-compare-conditional -Wno-deprecated-anon-enum-enum-conversion
DEFS ?= 
LIBS ?= 
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
OPTS += -ffast-math -funsafe-math-optimizations # potentially wrong results - check?
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
	@ $(call generate_source,main.cpp)
	@ $(CXX) $(FRAMEWORKS) $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) $(LPATH) -L./libs/raylib/src $(LIBS) -lseahorse -lraylib $@.cpp -o ./bin/$@
	@ $(call remove_source_files)
	@echo ${GREEN}[RUNNING]${NC}
	$(RUN) ./bin/$@ $(ARGS)

# Main.cpp file
release: main.cpp Makefile ./libs/seahorse/libseahorse.a
	@echo ${GREEN}[BUILDING]${NC} Release Version...
	@ $(call generate_source,main.cpp)
	@ $(CXX) main.cpp $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) $(LPATH) $(LIBS) -lseahorse -o ./bin/$@
	@ $(call remove_source_files)
	@echo ${GREEN}[RUNNING]${NC}
	$(RUN) ./bin/$@ $(ARGS)

# NB This must have EXACTLY the same architecture/optimisation flags set as the main build
libs/seahorse/libseahorse.a : $(shell find ./libs/seahorse -type f ! -name libseahorse.a ! -name libdebugseahorse.a) Makefile libs/eigen libs/spectra
	@echo ${GREEN}[BUILDING]${NC} Lib Seahorse...
	@ $(CXX) libs/seahorse/src/seahorse.cpp $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) -c -o libs/seahorse/libseahorse.o
	@ $(RUN) ar rvs libs/seahorse/libseahorse.a libs/seahorse/libseahorse.o
	@ $(RUN) rm libs/seahorse/libseahorse.o

# Main.cpp file in debug mode
debug: main.cpp Makefile ./libs/seahorse/libdebugseahorse.a
	@echo ${GREEN}[BUILDING]${NC} Debug Version...
	@ $(call generate_source,main.cpp)
	@ $(CXX) main.cpp $(STDS) $(DEFS) -UNDEBUG -O0 $(WFLAGS) $(IPATH) $(LPATH) $(LIBS) -ldebugseahorse -g -o ./bin/$@
	@ $(call remove_source_files)
	@echo ${GREEN}[RUNNING]${NC}
	$(RUN) lldb ./bin/$@ $(ARGS)

# The debugging version of the library
libs/seahorse/libdebugseahorse.a : $(shell find ./libs/seahorse -type f ! -name libseahorse.a ! -name libdebugseahorse.a) Makefile libs/eigen libs/spectra
	@echo ${GREEN}[BUILDING]${NC} Lib Debug Seahorse...
	@ $(CXX) libs/seahorse/src/seahorse.cpp $(STDS) $(DEFS) -UNDEBUG -O0 $(WFLAGS) $(IPATH) -c -g -o libs/seahorse/libdebugseahorse.o
	@ $(RUN) ar rvs libs/seahorse/libdebugseahorse.a libs/seahorse/libdebugseahorse.o
	@ $(RUN) rm libs/seahorse/libdebugseahorse.o


# We have this checked out at a specific time so don't need to check for changes
libs/raylib/src/libraylib.a : libs/raylib/src/raylib.h libs/raygui/src/raygui.h
	@echo ${GREEN}[BUILDING]${NC} Lib Raylib...
	@ (cd libs/raylib/src && make PLATFORM=PLATFORM_DESKTOP RAYLIB_MODULE_RAYGUI=TRUE)

# Clean up any executables
clean:
	@echo ${GREEN}[CLEANING]${NC} ...
	@- rm -rf ./bin/*
	@- rm libs/seahorse/libseahorse.a
	@- find libs/raylib/src/*.o | xargs rm

define generate_source
	@echo ${GREEN}[GENERATING SOURCE STRING]${NC} $(1)...
	@ $(RUN) xxd -i -u -n sourceFile $(1) > $(1).rawtext
endef