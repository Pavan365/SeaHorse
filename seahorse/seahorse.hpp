#pragma once

// only link library if we aren't debugging
#ifdef NDEBUG

// standard libs
#include <complex>
#include <format>
#include <iostream>

// Be careful - both declaration and definition to allow templated Eigen returns
#include "include/Physics/Vectors.hpp"

// Seahorse - just headers
#include "include/Optimisation/Basis/Basis.hpp"
#include "include/Optimisation/Cost/Cost.hpp"
#include "include/Optimisation/Optimiser.hpp"
#include "include/Optimisation/Stopper/Stopper.hpp"
#include "include/Optimisation/dCRAB.hpp"
#include "include/Physics/Hamiltonian.hpp"
#include "include/Physics/HilbertSpace.hpp"
#include "include/Physics/SplitStepper.hpp"
#include "include/Physics/Stepper.hpp"
#include "include/Utils/Globals.hpp"
#include "include/Utils/Random.hpp"

#else

#include "lib/seahorse.cpp"
#endif