#pragma once

#include "include/Physics/HilbertSpace.hpp"
#include "include/Physics/Potential.hpp"

#pragma GCC diagnostic push
#ifdef __GNUC__
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wuse-after-free"
#endif
#endif
#include <libs/eigen/Eigen/Dense>
#pragma GCC diagnostic pop

#include <libs/spectra/include/Spectra/MatOp/SparseSymShiftSolve.h>
#include <libs/spectra/include/Spectra/SymEigsShiftSolver.h>

class Spectrum {
public:
    RVec eigenvalues = RVec::Zero(2);
    int numEigs = -1;

    Eigen::MatrixXd eigenvectors = Eigen::MatrixXd::Zero(2, 2);
    int numEigvs = 0;

    // Empty constructor
    Spectrum() {};

    // Actual construction from result
    Spectrum(const RVec& eigs, const Eigen::MatrixXd& eigvs);

    // Return the eigenvalue (internal calls from Hamiltonian only)
    double eigenvalue(int i);
    // Return the eigenvector (internal calls from Hamiltonian only)
    const RVec eigenvector(int i);
};

class Hamiltonian {
public:
    Spectrum spectrum {};
    Eigen::SparseMatrix<double> H;
    // spatial potential - useful for referencing with respect to spectrum
    RVec V;

    // Constructor
    Hamiltonian(HilbertSpace& hs, const RVec& V);

    // Operator for getting/calculating eigenvectors
    RVec operator[](int i);

    // Calculation of only the eigenvalues
    double eigenvalue(int i);

    // Actually calculate Eigenvalues efficiently
    void calcEigenvalues();

    // Calculating the full spectrum of the matrix up to state `num`
    void calcSpectrum(int num, double smallest_eigenvalue = 0, bool looped = false);
};

// Class describing a controllable Hamiltonian which can be called to generate a specific Hamiltonian matrix
class HamiltonianFn {
public:
    HilbertSpace& hs;
    // We might as well store this since we need it for p^2/2m anyway
    RVec p;
    Potential V;
    // Shifted momentum space kinetic operator
    RVec T_p;

    // Constructor
    HamiltonianFn(HilbertSpace& hs, Potential V);

    // Construct with no potential
    HamiltonianFn(HilbertSpace& hs);

    // Evaluate HFn to H
    Hamiltonian operator()(double u);
};