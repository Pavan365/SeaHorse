STD = -std=c++20
# CXX = /usr/local/bin/g++-13
CXX = g++

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


###### ALL PROJECTS ######
project_files = $(notdir $(basename $(wildcard projects/*.cpp)))
all : $(project_files)
$(project_files) : % : projects/%.cpp
	@mkdir -p bin
	@printf "${GREEN}[BUILDING]${WHITE} $@...\n"
	$(CXX) $< $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(FLAGS) $(USE_MKL) -o ./bin/$(notdir $@)
	@printf "${GREEN}[BUILDING] Completed $@${WHITE}\n"

####### GUI VERSION #######
gui: bin/gui
bin/gui: gui/gui.cpp Makefile libs/raylib/src/libraylib.a
	@mkdir -p bin
	@printf "${GREEN}[BUILDING]${WHITE} Graphical Version...\n"
	@ $(CXX) $< $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(FLAGS) $(USE_MKL) $(FRAMEWORKS) $(LIBRAYLIB) -o ./bin/gui
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
# clear executables
	@- rm -r bin && mkdir -p bin
#  clear object files/libraries
	@- rm ./libs/raylib/src/libraylib.a