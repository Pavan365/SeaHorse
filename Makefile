STDS ?= -std=c++20
WFLAGS ?= -Wall -Wpedantic -Wno-deprecated-declarations -Wno-format-security -Wno-deprecated-enum-compare-conditional
DEFS ?= 
LIBS ?=
IPATH ?=
LPATH ?=
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
OPTS ?=
OPTS += -fno-math-errno
OPTS += -mavx2
OPTS += -mfma
OPTS += -march=native
DEFS += -DNDEBUG
OPTS += -ffp-contract=fast

# Compilers
CC = gcc
CXX = g++

# Defaults to main.cpp in release mode
all: clean release
	$(RUN) ./bin/release $(ARGS)

# Graphical Interface
gui:
	$(CXX) $(FRAMEWORKS) $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) -O3 $(LPATH) $(LIBS) ./libs/raylib/libraylib.a $@.cpp -g -o ./bin/$@

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