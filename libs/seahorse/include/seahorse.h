#pragma once

// standard libs
#include <complex>
#include <format>
#include <iostream>

// Be careful - both declaration and definition to allow templated Eigen returns
#include "libs/seahorse/src/Physics/Vectors.cpp"

// Seahorse - just headers
#include "libs/seahorse/include/Optimisation/Basis.h"
#include "libs/seahorse/include/Optimisation/Cost/Cost.h"
#include "libs/seahorse/include/Optimisation/Optimiser.h"
#include "libs/seahorse/include/Optimisation/Stopper/Stopper.h"
#include "libs/seahorse/include/Optimisation/dCRAB.h"
#include "libs/seahorse/include/Physics/Hamiltonian.h"
#include "libs/seahorse/include/Physics/HilbertSpace.h"
#include "libs/seahorse/include/Physics/SplitStepper.h"
#include "libs/seahorse/include/Physics/Stepper.h"
#include "libs/seahorse/include/Utils/Globals.h"
#include "libs/seahorse/include/Utils/Random.h"