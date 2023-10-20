STDS ?= -std=c++17
WFLAGS ?= -Wall -Wpedantic -Wno-deprecated-declarations
DEFS ?= 
LIBS ?=
IPATH ?=
LPATH ?=
OPTS ?=
FRAMEWORKS ?= -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

# Using libs...
IPATH += -I./libs
IPATH += -I./libs/eigen
IPATH += -I./include

# If using fftw3...
# We may or may not want this based on dimensions
# DEFS += -DEIGEN_FFTW_DEFAULT 
# LIBS += -lfftw3 -lfftw3f -lfftw3l

# Release optimisation flags
OPTS += -fno-math-errno
OPTS += -mavx2
OPTS += -mfma
OPTS += -march=native
OPTS += -DNDEBUG
OPTS += -ffp-contract=fast

# Compilers
CC = gcc
CXX = g++

# Defaults to main.cpp in release mode
all: clean release
	$(RUN) ./bin/release $(ARGS)

# Graphical Interface
gui:
	$(CXX) $(FRAMEWORKS) $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) -O3 ./libs/raylib/libraylib.a $@.cpp -g -o ./bin/$@

# NB: MUST BE [sources/objects] then [flags] then [output] OR LINKING FAILS
release: main.cpp Makefile
	$(info ************  RELEASE VERSION ************)
	$(CXX) main.cpp $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) -O3 $(LPATH) $(LIBS) -o ./bin/$@

# NB: MUST BE [sources/objects] then [flags] then [output] OR LINKING FAILS
debug: main.cpp Makefile
	$(info ************   DEBUG VERSION  ************)
	$(CXX) main.cpp $(STDS) $(DEFS) $(WFLAGS) $(IPATH) -O0 $(LPATH) $(LIBS) -o ./bin/$@
	$(RUN) ./bin/$@ $(ARGS)

# Clean up any executables
clean:
	@ rm -rf ./bin/*