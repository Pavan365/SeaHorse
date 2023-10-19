#ifndef SEAHORSE_
#define SEAHORSE_

// standard libs
#include <iostream>

// Eigen
// #include <fftw3.h>
#include <eigen/Eigen/Core>
#include <eigen/Eigen/Dense>
// #include <eigen/unsupported/Eigen/FFT>

// Spectra
#include <spectra/SymEigsSolver.h>
#include <spectra/MatOp/SparseSymMatProd.h>

// Data
#include "UbuntuMonoBold.h"
constexpr double PI = 3.141592653589793116;

// Seahorse
#include "Random.cpp"
#include "Vectors.cpp"
#include "HilbertSpace.cpp"
#include "Hamiltonian.cpp"


#endif // SEAHORSE_