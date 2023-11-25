# CMakeLists.txt for the seahorse library

# This allows dependency specific rebuilds

cmake_minimum_required(VERSION 3.10)
project(SeaHorse)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# List seahorse source files
set(SOURCE_FILES
        include/Physics/Vectors.hpp 
        include/Utils/Globals.hpp 
        src/Optimisation/Basis/Basis.cpp 
        src/Optimisation/Cost/ControlCost.cpp
        src/Optimisation/Cost/Cost.cpp 
        src/Optimisation/Cost/StateTransferCost.cpp 
        src/Optimisation/Optimiser.cpp 
        src/Optimisation/Stopper/StopComponent.cpp 
        src/Optimisation/Stopper/Stopper.cpp 
        src/Optimisation/dCRAB.cpp 
        src/Optimisation/Optimiser.cpp 
        src/Physics/HilbertSpace.cpp 
        src/Physics/Hamiltonian.cpp 
        src/Physics/Potential.cpp 
        src/Physics/Spline.cpp 
        src/Physics/SplitStepper.cpp 
        src/Physics/Stepper.cpp 
        src/Utils/Random.cpp 
# Add more source files as needed
)

# Warning flags
set(W_FLAGS -Wall)
list(APPEND W_FLAGS -Wpedantic)
list(APPEND W_FLAGS -Wno-deprecated-declarations)
list(APPEND W_FLAGS -Wno-format-security)
list(APPEND W_FLAGS -Wno-deprecated-enum-compare-conditional)
list(APPEND W_FLAGS -Wno-deprecated-anon-enum-enum-conversion)

set(OPTIMISE_FLAGS -Ofast)
list(APPEND OPTIMISE_FLAGS -DNDEBUG)
list(APPEND OPTIMISE_FLAGS -ffp-contract=fast)
list(APPEND OPTIMISE_FLAGS -mavx2)
list(APPEND OPTIMISE_FLAGS -mfma)
list(APPEND OPTIMISE_FLAGS -march=native)

# Linking external libraries
set(USE_FFTW -lfftw3 -lfftw3f -lfftw3l)

# LIB SEAHORSE 
add_library(seahorse STATIC ${SOURCE_FILES})

# Add warning flags to the project's compile options
target_compile_options(seahorse PUBLIC ${W_FLAGS})
# Add optimization flags to the project's compile options
target_compile_options(seahorse PUBLIC ${OPTIMISE_FLAGS})

# Link the FFTW library to the project
target_link_libraries(seahorse PUBLIC ${USE_FFTW})
target_compile_options(seahorse PUBLIC -DEIGEN_FFTW_DEFAULT)

# Add the project's include directories
target_include_directories(seahorse PUBLIC ${CMAKE_SOURCE_DIR}/../)
target_include_directories(seahorse PUBLIC ${CMAKE_SOURCE_DIR})
target_include_directories(seahorse PUBLIC ${CMAKE_SOURCE_DIR}/../libs/eigen)
target_include_directories(seahorse PUBLIC /usr/local/include)