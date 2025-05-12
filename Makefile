STD = -std=c++20
# CXX = g++

W_FLAGS = -Wall -Wextra
W_FLAGS+= -Wno-pragmas

USE_MKL = -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -lm -ldl -m64 -DEIGEN_USE_MKL_ALL -DMKL_ILP64

FLAGS = -ffp-contract=fast -mavx2 -mfma -march=native

INCLUDE_PATHS = -I./
INCLUDE_PATHS += -I./seahorse
INCLUDE_PATHS += -I/usr/include/mkl
INCLUDE_PATHS += -I./libs/eigen
INCLUDE_PATHS += -I./libs/raylib/src

LIBRAYLIB = -L./libs/raylib/src -lraylib
# macOS specific fixes
ifeq ($(shell uname), Darwin)
GUI = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL -lglfw
INCLUDE_PATHS += -I/opt/intel/oneapi/mkl/latest/include/
USE_MKL += -L/usr/local/lib -Wl,-rpath,/usr/local/lib -Wno-unused-command-line-argument
FLAGS += -O3
CXX = /usr/local/bin/g++-12
else
# Linux libraries
GUI = -lGL -lX11
FLAGS += -Ofast -fopenmp -lmkl_sequential
endif

# Colours for logging
RED=\033[1;91m
GREEN=\033[1;92m
ORANGE=\033[1;93m
WHITE=\033[1;0m

# Source files for seahorse library
SEAHORSE_SRCS := $(shell find seahorse/src -name "*.cpp")
SEAHORSE_OBJ_DIR := libs/seahorse
# Remove src/ from the path for cleaner structure
SEAHORSE_OBJS := $(patsubst seahorse/src/%.cpp,$(SEAHORSE_OBJ_DIR)/%.o,$(SEAHORSE_SRCS))
SEAHORSE_LIB := $(SEAHORSE_OBJ_DIR)/libseahorse.a

# Create directories if they don't exist
$(shell mkdir -p bin $(SEAHORSE_OBJ_DIR))

# Rule to create output directories for object files as needed
$(SEAHORSE_OBJS): | $(dir $(SEAHORSE_OBJS))

# Create needed directories
$(sort $(dir $(SEAHORSE_OBJS))):
	@mkdir -p $@

# Build rules for seahorse library with directory structure preserved (without src/)
$(SEAHORSE_OBJ_DIR)/%.o: seahorse/src/%.cpp Makefile
	@printf "${GREEN}[BUILDING]${WHITE} $@\n"
	@mkdir -p $(dir $@)
	@$(CXX) -c $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(FLAGS) $(USE_MKL) $< -o $@

# Build the static library
$(SEAHORSE_LIB): $(SEAHORSE_OBJS) Makefile
	@mkdir -p $(dir $@)
	@ar rcs $@ $(SEAHORSE_OBJS)
	@printf "${GREEN}[BUILDING] Completed seahorse library${WHITE}\n"

# Build rule for projects
project_files = $(notdir $(basename $(wildcard projects/*.cpp)))
all: $(project_files)

$(project_files): %: projects/%.cpp $(SEAHORSE_LIB)
	@mkdir -p bin
	@printf "${GREEN}[BUILDING]${WHITE} projects/$@\n"
	@$(CXX) $< $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(FLAGS) $(USE_MKL) $(SEAHORSE_LIB) -o ./bin/$(notdir $@)
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
	@printf "${GREEN}[BUILDING]${WHITE} Graphical Version\n"
	@ $(CXX) $< $(STD) $(INCLUDE_PATHS) $(W_FLAGS) $(FLAGS) $(USE_MKL) $(SEAHORSE_LIB) $(GUI) $(LIBRAYLIB) -o ./bin/gui
	@printf "${GREEN}[BUILDING] Completed gui${WHITE}\n"
	$(RUN) ./bin/gui $(ARGS)

# We have this checked out at a specific time so don't need to check for changes
libs/raylib/src/libraylib.a :
	@printf "${GREEN}[BUILDING]${WHITE} Lib Raylib\n"
	@ (cd libs/raylib/src && $(MAKE) PLATFORM=PLATFORM_DESKTOP RAYLIB_MODULE_RAYGUI=TRUE CUSTOM_CFLAGS="-Wno-unused-function -Wno-unused-but-set-variable -Wno-null-character")

####### CLEAN UP #######
.PHONY: clean
clean:
	@printf "${GREEN}[CLEANING]${WHITE} Cleaning\n"
	@- rm -rf bin $(SEAHORSE_OBJ_DIR)
	@- rm -f ./libs/raylib/src/libraylib.a