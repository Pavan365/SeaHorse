#pragma once

// standard libs
#include <complex>
#include <format>
#include <iostream>
using namespace std::complex_literals;

// Be careful - both declaration and definition to allow templated Eigen returns
#include "libs/seahorse/src/Vectors.cpp"

// Seahorse - just headers
#include "libs/seahorse/include/Basis.h"
#include "libs/seahorse/include/Globals.h"
#include "libs/seahorse/include/Hamiltonian.h"
#include "libs/seahorse/include/HilbertSpace.h"
#include "libs/seahorse/include/Optimiser.h"
#include "libs/seahorse/include/Random.h"
#include "libs/seahorse/include/SplitStepper.h"
#include "libs/seahorse/include/Stepper.h"
#include "libs/seahorse/include/dCRAB.h"