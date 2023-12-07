STD = -std=c++20 -DEIGEN_FFTW_DEFAULT
CXX = /usr/local/bin/g++-13
# CXX = g++

W_FLAGS = -Wall -Wextra
# W_FLAGS+= -Wno-unknown-warning-option
W_FLAGS+= -Wno-pragmas

OPTIMISE_FLAGS = -Ofast -ffp-contract=fast -DNDEBUG
OPTIMISE_FLAGS+= -mavx2 -mfma -march=native
LIBSEAHORSE = -L./seahorse/ -lseahorse

DEBUG_FLAGS = -O0 -DDEBUG

USE_FFTW = -lfftw3 -lfftw3f -lfftw3l -L/usr/local/lib
USE_MKL = -DEIGEN_USE_MKL_ALL -DMKL_ILP64 -fopenmp -m64 -L/usr/local/lib
USE_MKL += -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -lm -ldl

INCLUDE_PATHS = -I./
INCLUDE_PATHS += -I./seahorse
INCLUDE_PATHS += -I./libs/eigen
INCLUDE_PATHS += -I./libs/raylib/src
INCLUDE_PATHS += -I/usr/local/include

LIBRAYLIB = -L./libs/raylib/src -lraylib
FRAMEWORKS = -framework CoreVideo
FRAMEWORKS+= -framework IOKit
FRAMEWORKS+= -framework Cocoa
FRAMEWORKS+= -framework GLUT
FRAMEWORKS+= -framework OpenGL

# Colours for logging
RED=\033[1;91m
GREEN=\033[1;92m
ORANGE=\033[1;93m
WHITE=\033[1;0m


###### ALL PROJECTS ######
project_files = $(notdir $(basename $(wildcard projects/*.cpp)))
debug_files = $(patsubst %, %.debug, $(project_files))

all : $(project_files)
debug: $(debug_files)

$(project_files) : % : projects/%.cpp seahorse/libseahorse.a
	@mkdir -p bin
	@printf "${GREEN}[BUILDING]${WHITE} Release version of $@...\n"
	@ $(CXX) $< $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(OPTIMISE_FLAGS) $(USE_MKL) $(USE_FFTW)  $(LIBSEAHORSE) -o ./bin/$(notdir $@)
	@printf "${GREEN}[BUILDING] Completed $@${WHITE}\n"

$(debug_files) : %.debug : projects/%.cpp
	@mkdir -p bin
	@printf "${GREEN}[BUILDING]${WHITE} Debug version of $(basename $@)...\n"
	@ $(CXX) $< $(STD) $(W_FLAGS) $(DEBUG_FLAGS) $(USE_MKL) $(USE_FFTW) $(INCLUDE_PATHS) -g -o ./bin/$(notdir $(basename $@))_debug
	@printf "${GREEN}[BUILDING] Completed $@${WHITE}\n"

####### GUI VERSION #######
# Graphical Interface - (this is also inserted into the seahorse.app)
gui: bin/gui
bin/gui: gui/gui.cpp Makefile seahorse/libseahorse.a libs/raylib/src/libraylib.a
	@mkdir -p bin
	@printf "${GREEN}[BUILDING]${WHITE} Graphical Version...\n"
	@ $(CXX) $(FRAMEWORKS) $(STD) $(W_FLAGS) $(OPTIMISE_FLAGS) $(USE_MKL) $(USE_FFTW) $(INCLUDE_PATHS) $(LIBRAYLIB) $(LIBSEAHORSE) gui/gui.cpp -o ./bin/gui
	@printf "${GREEN}[BUILDING] Completed gui${WHITE}\n"
	$(RUN) ./bin/gui $(ARGS)

####### LIBRARIES #######
# NB These must have EXACTLY the same architecture/optimisation flags set as the project builds
# We have this checked out at a specific time so don't need to check for changes
libs/raylib/src/libraylib.a :
	@printf "${GREEN}[BUILDING]${WHITE} Lib Raylib...\n"
	@ (cd libs/raylib/src && make PLATFORM=PLATFORM_DESKTOP RAYLIB_MODULE_RAYGUI=TRUE CUSTOM_CFLAGS="-Wno-unused-function -Wno-unused-but-set-variable")

seahorse/Makefile: seahorse/CMakeLists.txt
	@printf "${GREEN}[BUILDING]${WHITE} Initialising Lib Seahorse...\n"
	@cmake -S $(<D) -B $(@D)

.PHONY: seahorse/libseahorse.a  # Always run to ensure cmake checks dependencies
seahorse/libseahorse.a: seahorse/Makefile
	@printf "${GREEN}[BUILDING]${WHITE} Lib Seahorse...\n"
	@$(MAKE) -C $(@D)

####### CLEAN UP #######
.PHONY: clean
clean:
	@printf "${GREEN}[CLEANING]${WHITE} ...\n"
# clear executables
	@- rm -r bin && mkdir -p bin
#  clear object files/libraries
	@- find ./seahorse -type f -name '*.[oa]' | xargs rm
	@- rm ./libs/raylib/src/libraylib.a
# clear cmake files
	@- cd seahorse && make clean && cmake . --fresh
# these are incase --fresh isn't supported
	@- cd seahorse && make clean