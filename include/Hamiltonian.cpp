#ifndef SEAHORSE_HAMILTONIAN
#define SEAHORSE_HAMILTONIAN
#include "HilbertSpace.h"

// Spectra
#include <spectra/SymEigsSolver.h>
#include <spectra/MatOp/SparseSymMatProd.h>

class Spectrum
{
public:
    RVec eigenvalues;
    Eigen::MatrixXd eigenvectors;
    int num;

    Spectrum(RVec eigs, Eigen::MatrixXd eigvs) : eigenvalues(eigs), eigenvectors(eigvs), num(eigs.size()) {}

    double eigenvalue(int i)
    {
        if (i < 0 || i > num - 1)
            std::cerr << "Invalid eigenvalue" << std::endl;
        return eigenvalues[num - i - 1];
    }
    RVec eigenvector(int i)
    {
        if (i < 0 || i > num - 1)
            std::cerr << "Invalid eigenvector" << std::endl;
        return (Eigen::VectorXd)eigenvectors.col(num - i - 1);
    }
};

// This is a spatial Hamiltonian Matrix (real valued potentials)
class Hamiltonian
{
private:
    HilbertSpace &hs;

public:
    Eigen::SparseMatrix<double> H;

    // Components of H
    RVec V;
    Eigen::SparseMatrix<double> T;

    // Constructor
    Hamiltonian(HilbertSpace &hs, RVec V) : hs(hs), V(V), T(hs.T())
    {
        H = T + V._vec.asDiagonal().toDenseMatrix().sparseView();
    }

    RVec eigenvalues()
    {
        // Finds all eigenvalues for the hamiltonian
        auto temp = T.toDense();
        H.is
        Eigen::VectorXd diag = temp.diagonal().array() + V._vec.array();
        Eigen::VectorXd subdiag = temp.diagonal(1);

        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(diag.size());
        eigensolver.computeFromTridiagonal(diag, subdiag, Eigen::EigenvaluesOnly);

        if (eigensolver.info() != Eigen::Success)
            std::cout << "Failed to calculate full spectrum";

        return eigensolver.eigenvalues();
    }

    Spectrum spectrum(int num)
    {
        // 'Spectra' implementation finds first 'num' eigenvalues/eigenvectors

        auto nev = std::max(std::min(num, (int)H.cols() - 1), 1);
        auto ncv = std::min(4 * num + 10, (int)H.cols());
        std::cout << "num: " << num << ", nev: " << nev << ", ncv:" << ncv << std::endl;
        // Construct matrix operation object using the wrapper class SparseSymMatProd
        Spectra::SparseSymMatProd<double> op(H);
        // Construct eigen solver object, requesting the smallest 'num' eigenvalues
        Spectra::SymEigsSolver<Spectra::SparseSymMatProd<double>> eigs(op, nev, ncv);

        // Initialize and compute
        eigs.init();
        int nconv = eigs.compute(Spectra::SortRule::SmallestAlge);

        // Retrieve results
        if (eigs.info() == Spectra::CompInfo::Successful)
        {
            return Spectrum(eigs.eigenvalues(), eigs.eigenvectors());
        }
        else if (eigs.info() == Spectra::CompInfo::NotConverging)
        {
            std::cerr << "Spectrum Not Converging, only " << nconv << " eigenvalues" << std::endl;
            return Spectrum(eigs.eigenvalues(), eigs.eigenvectors());
        }
        else if (eigs.info() == Spectra::CompInfo::NumericalIssue)
        {
            std::cerr << "Spectrum Numerical Issue" << std::endl;
        }
        else if (eigs.info() == Spectra::CompInfo::NotComputed)
        {
            std::cerr << "Spectrum Not Computed" << std::endl;
        }

        // Return empty Spectrum
        return Spectrum(RVec(0), {});
    }
};

// Class describing a controllable Hamiltonian which can be called to generate a specific Hamiltonian matrix
class HamiltonianFn
{
public:
    HilbertSpace &hs;
    std::function<RVec(double)> V;

    HamiltonianFn(HilbertSpace &hs, std::function<RVec(double)> V) : hs(hs), V(V) {}
    HamiltonianFn(HilbertSpace &hs) : HamiltonianFn(hs, [dim = hs.dim()](double u){return RVec(dim, 0);}) {}

    Hamiltonian operator()(double u){return Hamiltonian(hs, V(u));}
};

#endif // SEAHORSE_HAMILTONIAN