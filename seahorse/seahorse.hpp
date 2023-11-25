#pragma once

// standard libs
#include <complex>
// #include <format>
#include <iostream>

// link library if we aren't debugging
#ifdef DEBUG
// Be careful - both declaration and definition to allow templated Eigen returns
#include "include/Physics/Vectors.hpp"
#include "include/Utils/Globals.hpp"
#include "include/Utils/Timer.hpp"

#include "src/Optimisation/Basis/Basis.cpp"
#include "src/Optimisation/Cost/ControlCost.cpp"
#include "src/Optimisation/Cost/Cost.cpp"
#include "src/Optimisation/Cost/StateTransferCost.cpp"
#include "src/Optimisation/Optimiser.cpp"
#include "src/Optimisation/Stopper/StopComponent.cpp"
#include "src/Optimisation/Stopper/Stopper.cpp"
#include "src/Optimisation/dCRAB.cpp"
#include "src/Physics/Hamiltonian.cpp"
#include "src/Physics/HilbertSpace.cpp"
#include "src/Physics/Potential.cpp"
#include "src/Physics/Spline.cpp"
#include "src/Physics/SplitStepper.cpp"
#include "src/Physics/Stepper.cpp"
#include "src/Utils/Random.cpp"
#else

// Be careful - both declaration and definition to allow templated Eigen returns
#include "include/Physics/Vectors.hpp"
#include "include/Utils/Globals.hpp"
#include "include/Utils/Timer.hpp"

// Seahorse - just headers
#include "include/Optimisation/Basis/Basis.hpp"
#include "include/Optimisation/Cost/ControlCost.hpp"
#include "include/Optimisation/Cost/Cost.hpp"
#include "include/Optimisation/Cost/StateTransferCost.hpp"
#include "include/Optimisation/Optimiser.hpp"
#include "include/Optimisation/Stopper/StopComponent.hpp"
#include "include/Optimisation/Stopper/Stopper.hpp"
#include "include/Optimisation/dCRAB.hpp"
#include "include/Physics/Hamiltonian.hpp"
#include "include/Physics/HilbertSpace.hpp"
#include "include/Physics/Potential.hpp"
#include "include/Physics/Spline.hpp"
#include "include/Physics/SplitStepper.hpp"
#include "include/Physics/Stepper.hpp"
#include "include/Utils/Random.hpp"

#endif