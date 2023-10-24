#pragma once

// standard libs
#include <iostream>
#include <format>
#include <complex>
using namespace std::complex_literals;

// Seahorse - definitions+declarations so must all be static/inline etc.
#include "seahorse/src/Globals.cpp"
#include "seahorse/src/Vectors.cpp"

// Seahorse - just headers
#include "seahorse/include/Random.h"
#include "seahorse/include/HilbertSpace.h"
#include "seahorse/include/Hamiltonian.h"
#include "seahorse/include/Steppers.h"
