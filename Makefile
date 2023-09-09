STDS ?= -std=c++17
WFLAGS ?= -Wall -Wpedantic
DEFS ?= 
LIBS ?=
IPATH ?=
LPATH ?=
OPTS ?=

# Using libs...
IPATH += -I./libs
IPATH += -I./libs/eigen

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


# RUNS AFTER MAKING SURE IT IS COMPILED AND UP TO DATE
all: release
	$(RUN) ./bin/release $(ARGS)

# TIMES IN RELEASE MODE
time: release
	time ./bin/release $(ARGS)

# Raylib testing
gui:
	clang -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL $(STDS) $(IPATH) ./libs/raylib/libraylib.a $@.cpp -o ./bin/$@
	$(RUN) ./bin/$@ $(ARGS)

# NB: MUST BE [sources/objects] then [flags] then [output] OR LINKING FAILS
release: main.cpp Makefile
	$(info ************  RELEASE VERSION ************)
	$(CXX) main.cpp $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) -O3 $(LPATH) $(LIBS) -o ./bin/$@

# NB: MUST BE [sources/objects] then [flags] then [output] OR LINKING FAILS
test: main.cpp Makefile
	$(info ************   TEST VERSION   ************)
	$(CXX) main.cpp $(STDS) $(DEFS) $(WFLAGS) $(IPATH) -O3 $(LPATH) $(LIBS) -o ./bin/$@
	$(RUN) ./bin/$@ $(ARGS)
# NB: MUST BE [sources/objects] then [flags] then [output] OR LINKING FAILS
debug: main.cpp Makefile
	$(info ************   DEBUG VERSION  ************)
	$(CXX) main.cpp $(STDS) $(DEFS) $(WFLAGS) $(IPATH) -O0 $(LPATH) $(LIBS) -o ./bin/$@
	$(RUN) ./bin/$@ $(ARGS)

# Clean up any executables
clean:
	@ rm -rf ./bin/*