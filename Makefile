STD = -std=c++20
# CXX = /usr/local/bin/g++-13
# CXX = g++

W_FLAGS = -Wall -Wextra
# W_FLAGS+= -Wno-unknown-warning-option
W_FLAGS+= -Wno-pragmas

USE_MKL = -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_sequential -lmkl_core -liomp5 -lpthread -lm -ldl -DEIGEN_USE_MKL_ALL -fopenmp -DMKL_ILP64 -m64

# FLAGS = -O0
FLAGS = -Ofast -ffp-contract=fast -mavx2 -mfma -march=native

INCLUDE_PATHS = -I./
INCLUDE_PATHS += -I./seahorse
INCLUDE_PATHS += -I/usr/include/mkl
INCLUDE_PATHS += -I./libs/eigen
INCLUDE_PATHS += -I./libs/raylib/src

LIBRAYLIB = -L./libs/raylib/src -lraylib
# macOS frameworks - not used on Linux
# FRAMEWORKS = -framework CoreVideo
# FRAMEWORKS+= -framework IOKit
# FRAMEWORKS+= -framework Cocoa
# FRAMEWORKS+= -framework GLUT
# FRAMEWORKS+= -framework OpenGL
# Linux libraries
FRAMEWORKS = -lGL -lX11

# Colours for logging
RED=\033[1;91m
GREEN=\033[1;92m
ORANGE=\033[1;93m
WHITE=\033[1;0m

# Source files for seahorse library
SEAHORSE_SRCS := $(shell find seahorse/src -name "*.cpp")
SEAHORSE_OBJS := $(SEAHORSE_SRCS:.cpp=.o)
SEAHORSE_LIB := ./lib/libseahorse.a

# Create directories if they don't exist
$(shell mkdir -p bin lib)

# Build rules for seahorse library
$(SEAHORSE_OBJS): %.o: %.cpp
	@printf "${GREEN}[BUILDING]${WHITE} $@...\n"
	@$(CXX) -c $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(FLAGS) $< -o $@

# Build the static library
$(SEAHORSE_LIB): $(SEAHORSE_OBJS)
	@printf "${GREEN}[BUILDING]${WHITE} seahorse library...\n"
	@mkdir -p lib
	@ar rcs $@ $^
	@printf "${GREEN}[BUILDING] Completed seahorse library${WHITE}\n"

# Build rule for projects
project_files = $(notdir $(basename $(wildcard projects/*.cpp)))
all: $(project_files)

$(project_files): %: projects/%.cpp $(SEAHORSE_LIB)
	@mkdir -p bin
	@printf "${GREEN}[BUILDING]${WHITE} $@...\n"
	$(CXX) $< $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(FLAGS) $(USE_MKL) $(SEAHORSE_LIB) -o ./bin/$(notdir $@)
	@printf "${GREEN}[BUILDING] Completed $@${WHITE}\n"

# Force rebuild of the library
.PHONY: rebuild_lib
rebuild_lib:
	@rm -f $(SEAHORSE_OBJS) $(SEAHORSE_LIB)
	@$(MAKE) $(SEAHORSE_LIB)

####### GUI VERSION #######
gui: bin/gui
bin/gui: gui/gui.cpp $(SEAHORSE_LIB) Makefile libs/raylib/src/libraylib.a
	@mkdir -p bin
	@printf "${GREEN}[BUILDING]${WHITE} Graphical Version...\n"
	@ $(CXX) $< $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(FLAGS) $(USE_MKL) $(FRAMEWORKS) $(LIBRAYLIB) $(SEAHORSE_LIB) -o ./bin/gui
	@printf "${GREEN}[BUILDING] Completed gui${WHITE}\n"
	$(RUN) ./bin/gui $(ARGS)

# We have this checked out at a specific time so don't need to check for changes
libs/raylib/src/libraylib.a :
	@printf "${GREEN}[BUILDING]${WHITE} Lib Raylib...\n"
	@ (cd libs/raylib/src && $(MAKE) PLATFORM=PLATFORM_DESKTOP RAYLIB_MODULE_RAYGUI=TRUE CUSTOM_CFLAGS="-Wno-unused-function -Wno-unused-but-set-variable")

####### CLEAN UP #######
.PHONY: clean
clean:
	@printf "${GREEN}[CLEANING]${WHITE} ...\n"
	@- rm -rf bin lib
	@- rm -f $(SEAHORSE_OBJS)
	@- mkdir -p bin lib
	@- rm ./libs/raylib/src/libraylib.a