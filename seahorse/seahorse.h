#pragma once

// standard libs
#include <complex>
#include <format>
#include <iostream>

// Be careful - both declaration and definition to allow templated Eigen returns
#include "src/Physics/Vectors.cpp"

// Seahorse - just headers
#include "include/Optimisation/Basis/Basis.h"
#include "include/Optimisation/Cost/Cost.h"
#include "include/Optimisation/Optimiser.h"
#include "include/Optimisation/Stopper/Stopper.h"
#include "include/Optimisation/dCRAB.h"
#include "include/Physics/Hamiltonian.h"
#include "include/Physics/HilbertSpace.h"
#include "include/Physics/SplitStepper.h"
#include "include/Physics/Stepper.h"
#include "include/Utils/Globals.h"
#include "include/Utils/Random.h"