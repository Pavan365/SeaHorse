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

# If using matplotlibcpp header file...
DEFS += -DWITHOUT_NUMPY
IPATH += -I./libs/matplotlib-cpp
IPATH += -I/usr/local/Cellar/python@3.11/3.11.4/Frameworks/Python.framework/Versions/3.11/include/python3.11
LPATH += -L/usr/local/Cellar/python@3.11/3.11.4/Frameworks/Python.framework/Versions/3.11/lib
LIBS += -lpython3.11

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
all: test
	$(RUN) ./bin/test $(ARGS)

# TIMES IN RELEASE MODE
time: release
	time ./bin/release $(ARGS)

# NB: MUST BE [sources/objects] then [flags] then [output] OR LINKING FAILS
release: main.cpp Makefile
	$(info ************  RELEASE VERSION ************)
	$(CXX) main.cpp $(STDS) $(DEFS) $(OPTS) $(WFLAGS) $(IPATH) -O3 $(LPATH) $(LIBS) -o ./bin/$@

# NB: MUST BE [sources/objects] then [flags] then [output] OR LINKING FAILS
test: main.cpp Makefile
	$(info ************   TEST VERSION   ************)
	$(CXX) main.cpp $(STDS) $(DEFS) $(WFLAGS) $(IPATH) -O3 $(LPATH) $(LIBS) -o ./bin/$@

# NB: MUST BE [sources/objects] then [flags] then [output] OR LINKING FAILS
debug: main.cpp Makefile
	$(info ************   DEBUG VERSION  ************)
	$(CXX) main.cpp $(STDS) $(DEFS) $(WFLAGS) $(IPATH) -O0 $(LPATH) $(LIBS) -o ./bin/$@
	$(RUN) ./bin/$@ $(ARGS)

# Clean up any executables
clean:
	@ rm -rf ./bin/*