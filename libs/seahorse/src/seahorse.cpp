// standard libs
#include <complex>
#include <format>
#include <iostream>

// Seahorse Components - actual implementations
#include "libs/seahorse/include/Utils/Globals.h"
#include "libs/seahorse/src/Optimisation/Basis.cpp"
#include "libs/seahorse/src/Physics/Hamiltonian.cpp"
#include "libs/seahorse/src/Physics/HilbertSpace.cpp"
#include "libs/seahorse/src/Optimisation/Optimiser.cpp"
#include "libs/seahorse/src/Utils/Random.cpp"
#include "libs/seahorse/src/Physics/SplitStepper.cpp"
#include "libs/seahorse/src/Physics/Stepper.cpp"
#include "libs/seahorse/src/Optimisation/Stopper/Stopper.cpp"
#include "libs/seahorse/src/Optimisation/Cost/Cost.cpp"
#include "libs/seahorse/src/Physics/Vectors.cpp"
#include "libs/seahorse/src/Optimisation/dCRAB.cpp"