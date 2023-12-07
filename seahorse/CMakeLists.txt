# CMakeLists.txt for the seahorse library

# This allows dependency specific rebuilds

cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_COMPILER "/usr/local/bin/g++-13")

project(SeaHorse)

# List seahorse source files
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)

# Warning flags
set(W_FLAGS -Wall)
list(APPEND W_FLAGS -Wextra)
# list(APPEND W_FLAGS -Wno-unknown-warning-option)
list(APPEND W_FLAGS -Wno-pragmas)

set(OPTIMISE_FLAGS -Ofast)
list(APPEND OPTIMISE_FLAGS -DNDEBUG)
list(APPEND OPTIMISE_FLAGS -ffp-contract=fast)
list(APPEND OPTIMISE_FLAGS -mavx2)
list(APPEND OPTIMISE_FLAGS -mfma)
list(APPEND OPTIMISE_FLAGS -march=native)


# LIB SEAHORSE 
add_library(seahorse STATIC ${SOURCE_FILES})

# Add warning flags to the project's compile options
target_compile_options(seahorse PUBLIC ${W_FLAGS})
# Add optimization flags to the project's compile options
target_compile_options(seahorse PUBLIC ${OPTIMISE_FLAGS})

# Linking the MKL library
set(USE_MKL -lmkl_intel_ilp64 -lmkl_intel_thread -lmkl_core -liomp5 -lpthread -lm -ldl)
set(MKL_FLAGS -DEIGEN_USE_MKL_ALL)
list(APPEND MKL_FLAGS -fopenmp)
list(APPEND MKL_FLAGS -DMKL_ILP64)
list(APPEND MKL_FLAGS -m64)
target_compile_options(seahorse PUBLIC ${MKL_FLAGS})
target_link_libraries(seahorse PUBLIC ${USE_MKL})

# Linking the FFTW library
set(USE_FFTW -lfftw3 -lfftw3f -lfftw3l)
# Link the FFTW library to the project
target_link_libraries(seahorse PUBLIC ${USE_FFTW})
target_compile_options(seahorse PUBLIC -DEIGEN_FFTW_DEFAULT)

# Add the project's include directories
target_include_directories(seahorse PUBLIC ${CMAKE_SOURCE_DIR}/../)
target_include_directories(seahorse PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(seahorse PUBLIC ${CMAKE_SOURCE_DIR}/../libs/eigen)

# g++ specific flags to find the global libraries
target_include_directories(seahorse PUBLIC /usr/local/include)
target_link_directories(seahorse PUBLIC /usr/local/lib)

