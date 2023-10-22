#ifndef SEAHORSE_
#define SEAHORSE_

// standard libs
#include <iostream>
#include <format>
#include <complex>
using namespace std::complex_literals;

// Eigen
// #include <fftw3.h>
#include <eigen/Eigen/Core>
#include <Eigen/SparseCore>
#include <eigen/Eigen/Dense>
// #include <eigen/unsupported/Eigen/FFT>

// Data
#include "UbuntuMonoBold.h"

// Seahorse
#include "Globals.h"
#include "Random.h"
#include "Vectors.h"
#include "Hamiltonian.h"
#include "Solvers.h"

#endif // SEAHORSE_