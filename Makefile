STD = -std=c++20 -DEIGEN_FFTW_DEFAULT
# CXX = /usr/local/bin/g++-13
CXX = g++

W_FLAGS = -Wall -Wextra -Werror
W_FLAGS+= -Wno-unknown-warning-option
W_FLAGS+= -Wno-pragmas

OPTIMISE_FLAGS = -Ofast -ffp-contract=fast -DNDEBUG
OPTIMISE_FLAGS+= -mavx2 -mfma -march=native
LIBSEAHORSE = -L./seahorse/ -lseahorse

DEBUG_FLAGS = -O0 -DDEBUG

USE_FFTW = -lfftw3 -lfftw3f -lfftw3l -L/usr/local/lib

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
GREEN='\033[92m'
RED='\033[91m'
NC='\033[0m'


###### ALL PROJECTS ######
project_files = $(notdir $(basename $(wildcard projects/*.cpp)))
debug_files = $(patsubst %, %.debug, $(project_files))

all : $(project_files)
debug: $(debug_files)

$(project_files) : % : projects/%.cpp seahorse/libseahorse.a
	@mkdir -p bin
	@echo ${GREEN}[BUILDING]${NC} Release version of $@...
	@ $(CXX) $< $(STD) $(W_FLAGS) $(OPTIMISE_FLAGS) $(INCLUDE_PATHS) $(USE_FFTW) $(LIBSEAHORSE) -o ./bin/$(notdir $@)
	@echo ${GREEN}[BUILDING] Completed $@${NC}

$(debug_files) : %.debug : projects/%.cpp
	@mkdir -p bin
	@echo ${GREEN}[BUILDING]${NC} Debug version of $(basename $@)...
	@ $(CXX) $< $(STD) $(W_FLAGS) $(DEBUG_FLAGS) $(INCLUDE_PATHS) $(USE_FFTW) -g -o ./bin/$(notdir $(basename $@))_debug
	@echo ${GREEN}[BUILDING] Completed $@${NC}

####### GUI VERSION #######
# Graphical Interface - (this is also inserted into the seahorse.app)
gui: bin/gui
bin/gui: gui/gui.cpp Makefile seahorse/libseahorse.a libs/raylib/src/libraylib.a
	@mkdir -p bin
	@echo ${GREEN}[BUILDING]${NC} Graphical Version...
	@ $(CXX) $(FRAMEWORKS) $(STD) $(W_FLAGS) $(OPTIMISE_FLAGS) $(INCLUDE_PATHS) $(LIBRAYLIB) $(USE_FFTW) $(LIBSEAHORSE) gui/gui.cpp -o ./bin/gui
	@echo ${GREEN}[BUILDING] Completed gui${NC}
	$(RUN) ./bin/gui $(ARGS)

####### LIBRARIES #######
# We have this checked out at a specific time so don't need to check for changes
libs/raylib/src/libraylib.a :
	@echo ${GREEN}[BUILDING]${NC} Lib Raylib...
	@ (cd libs/raylib/src && make PLATFORM=PLATFORM_DESKTOP RAYLIB_MODULE_RAYGUI=TRUE CUSTOM_CFLAGS="-Wno-unused-function -Wno-unused-but-set-variable")

# NB This must have EXACTLY the same architecture/optimisation flags set as the main build
seahorse/Makefile: seahorse/CMakeLists.txt
	@echo ${GREEN}[BUILDING]${NC} Initialising Lib Seahorse...
	@cmake -S $(<D) -B $(@D)

.PHONY: seahorse/libseahorse.a  # Always run to ensure cmake checks dependencies
seahorse/libseahorse.a: seahorse/Makefile
	@echo ${GREEN}[BUILDING]${NC} Lib Seahorse...
	@$(MAKE) -C $(@D) -j 8

####### CLEAN UP #######
.PHONY: clean
clean:
	@echo ${GREEN}[CLEANING]${NC} ...
	@- rm -r bin && mkdir -p bin
	@- find ./seahorse -type f -name '*.[oa]' | xargs rm
	@- rm ./libs/raylib/src/libraylib.a
	@- cd seahorse && make clean && cmake . --fresh