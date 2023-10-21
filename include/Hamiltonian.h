#ifndef SEAHORSE_HAMILTONIAN
#define SEAHORSE_HAMILTONIAN
#include "HilbertSpace.h"

// Spectra
#include <spectra/SymEigsSolver.h>
#include <spectra/MatOp/SparseSymMatProd.h>

class Spectrum
{
public:
    RVec eigenvalues{};
    int numEigs = 0;

    Eigen::MatrixXd eigenvectors{};
    int numEigvs = 0;

    Spectrum(){}
    Spectrum(RVec eigs, Eigen::MatrixXd eigvs) : eigenvalues(eigs), numEigs(eigs.size()), eigenvectors(eigvs), numEigvs(eigvs.cols()) {}

    double eigenvalue(int i)
    {
        if (i < 0 || i > numEigs - 1) std::cerr << "Invalid eigenvalue" << std::endl;
        return eigenvalues[i];
    }
    const auto eigenvector(int i)
    {
        if (i < 0 || i > numEigvs - 1) std::cerr << "Invalid eigenvector" << std::endl;
        return (Eigen::VectorXd)eigenvectors.col(i);
    }
};

// This is a spatial Hamiltonian Matrix (real valued potentials)
class Hamiltonian
{
private:
    HilbertSpace &hs;
public:
    Spectrum spectrum{};
    Eigen::SparseMatrix<double> H;

    // Components of H
    RVec V;
    Eigen::SparseMatrix<double> T;

    // Constructor
    Hamiltonian(HilbertSpace &hs, RVec V) : hs(hs), V(V), T(hs.T())
    {
        H = T + V._vec.asDiagonal().toDenseMatrix().sparseView();
    }

    // Operator for obtaining eigenstates
    auto operator[](int i){
        if (i>=spectrum.numEigvs) // We have't already calculated enough states
        {
            // NB: i+1 as we zero index so the 0th is calcSpectrum(1)
            calcSpectrum(i+1);
        }
        return spectrum.eigenvector(i);
    }

    // Efficient calculation of only the eigenvalues
    double eigenvalue(int i){
        if (i>=spectrum.numEigs) // We've only used calcSpectrum so haven't calculated enough eigenvalues so get all of them quickly
        {
            calcEigenvalues();
        }
        return spectrum.eigenvalue(i);
    }

    void calcEigenvalues()
    {
        // Finds all eigenvalues for the hamiltonian
        auto temp = T.toDense();

        Eigen::VectorXd diag = temp.diagonal().array() + V._vec.array();
        Eigen::VectorXd subdiag = temp.diagonal(1);

        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(diag.size());
        eigensolver.computeFromTridiagonal(diag, subdiag, Eigen::EigenvaluesOnly);

        if (eigensolver.info() != Eigen::Success) std::cerr << "Failed to calculate full spectrum";

        spectrum.eigenvalues = RVec(eigensolver.eigenvalues());
        spectrum.numEigs = spectrum.eigenvalues.size();
    }

    // Calculating the full spectrum of the matrix up to state `num`
    void calcSpectrum(int num)
    {
        // 'Spectra' implementation finds first 'num' eigenvalues/eigenvectors of a sparse symmetric matrix
        auto nev = std::max(std::min(num, (int)H.cols() - 1), 1);
        auto ncv = std::min(4 * num + 10, (int)H.cols());

        // Construct matrix operation object using the wrapper class SparseSymMatProd
        Spectra::SparseSymMatProd<double> op(H);
        // Construct eigen solver object, requesting the smallest 'num' eigenvalues
        Spectra::SymEigsSolver<Spectra::SparseSymMatProd<double>> eigs(op, nev, ncv);
        // Initialize and compute
        eigs.init();
        int nconv = eigs.compute(Spectra::SortRule::SmallestAlge);

        // Retrieve results
        if (eigs.info() == Spectra::CompInfo::NotConverging)
        {
            std::cerr << "Spectrum Not Converging, only " << nconv << " eigenvalues" << std::endl;
        }
        else if (eigs.info() == Spectra::CompInfo::NumericalIssue)
        {
            std::cerr << "Spectrum Numerical Issue" << std::endl;
            return;
        }
        else if (eigs.info() == Spectra::CompInfo::NotComputed)
        {
            std::cerr << "Spectrum Not Computed" << std::endl;
            return;
        }
        // If we got values then update the spectrum
        if (eigs.info() == Spectra::CompInfo::Successful || eigs.info() == Spectra::CompInfo::NotConverging)
        {
            Eigen::VectorXd eig = eigs.eigenvalues().reverse();
            Eigen::MatrixXd eigv = eigs.eigenvectors().rowwise().reverse();
            
            if (eig.size()>spectrum.numEigs)
            {
                spectrum.eigenvalues = RVec(eig);
                spectrum.numEigs = eig.size();
            }
            if (eigv.cols()>spectrum.numEigvs)
            {
                spectrum.eigenvectors = eigv;
                spectrum.numEigvs = eigv.cols();
            }
        }
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