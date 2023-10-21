#ifndef SEAHORSE_
#define SEAHORSE_

// standard libs
#include <iostream>
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
constexpr double PI = 3.141592653589793116;

// Seahorse
#include "Random.h"
#include "Vectors.h"
#include "Hamiltonian.h"

#endif // SEAHORSE_