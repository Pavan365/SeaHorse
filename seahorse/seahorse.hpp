#pragma once

// standard libs
#include <complex>
// #include <format>
#include <iostream>

// Be careful with Vectors.hpp - both declaration and definition to allow templated Eigen returns
#include "src/Json/json.hpp"
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
#include "src/Physics/Time.hpp"
#include "src/Physics/Vectors.hpp"
#include "src/Utils/Globals.hpp"
#include "src/Utils/Includer.hpp"
#include "src/Utils/Logger.hpp"
#include "src/Utils/Random.cpp"
#include "src/Utils/Timer.cpp"