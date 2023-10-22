#ifndef SEAHORSE_HAMILTONIAN
#define SEAHORSE_HAMILTONIAN
#include "HilbertSpace.h"

// Spectra
#include <spectra/SymEigsSolver.h>
#include <spectra/MatOp/SparseSymMatProd.h>

class Spectrum
{
public:
    RVec eigenvalues = RVec::Zero(2);
    int numEigs = 0;

    Eigen::MatrixXd eigenvectors = Eigen::MatrixXd::Zero(2,2);
    int numEigvs = 0;

    Spectrum(){}
    Spectrum(RVec eigs, Eigen::MatrixXd eigvs) : eigenvalues(eigs), numEigs(eigs.size()), eigenvectors(eigvs), numEigvs(eigvs.cols()) {}

    double eigenvalue(int i)
    {
        if (i < 0 || i > numEigs - 1) {S_ERROR("Invalid eigenvalue");return 0;};
        return eigenvalues[i];
    }
    const RVec eigenvector(int i)
    {
        if (i < 0 || i > numEigvs - 1) {S_ERROR("Invalid eigenvector");return RVec::Zero(2);}
        return eigenvectors.col(i);
    }
};

class Hamiltonian
{
public:
    Spectrum spectrum{};
    Eigen::SparseMatrix<double> H;

    RVec V; // spatial potential - useful for referencing with respect to spectrum

    // Constructor
    Hamiltonian(HilbertSpace &hs, RVec V) : V(V) 
    {
        H = hs.T();
        H.diagonal()+=V;
    }

    // Operator for obtaining eigenstates
    RVec operator[](int i){
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
        // We cant use tridiag solver as we use a 4th order d2/dx2 approx
        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(H.size());

        eigensolver.compute(H,Eigen::EigenvaluesOnly);

        if (eigensolver.info() != Eigen::Success) std::cerr << "Failed to calculate full spectrum";

        spectrum.eigenvalues = RVec(eigensolver.eigenvalues());
        spectrum.numEigs = spectrum.eigenvalues.size();
    }

    // Calculating the full spectrum of the matrix up to state `num`
    void calcSpectrum(int num, bool looped = false)
    {
        // 'Spectra' implementation finds first 'num' eigenvalues/eigenvectors of a sparse symmetric matrix
        auto nev = std::max(std::min(num, (int)H.cols() - 1), 1);
        auto ncv = std::min(3 * num + 10, (int)H.cols());
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
            S_ERROR("calcSpectrum: Not Converging, only ", nconv, " eigenvalues");
        }
        else if (eigs.info() == Spectra::CompInfo::NumericalIssue)
        {
            S_ERROR("calcSpectrum: Spectra Numerical Issue");
        }
        else if (eigs.info() == Spectra::CompInfo::NotComputed)
        {
            S_ERROR("calcSpectrum: Spectra refused to compute spectrum?");
        }
        // If we got values then update the spectrum
        if (eigs.info() == Spectra::CompInfo::Successful || (eigs.info() == Spectra::CompInfo::NotConverging && nconv!=0))
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
        else if (looped==false) // It failed to get any eigenvectors so try again with more lax numbers
        { 
            S_INFO("Recalculating eigenspectrum with ",2*nev+5," number of states");
            calcSpectrum(2*nev+6, true);
        }
        return;
    }
};

// Class describing a controllable Hamiltonian which can be called to generate a specific Hamiltonian matrix
class HamiltonianFn
{
public:
    HilbertSpace &hs;
    RVec p; // We might as well store this since we need it for p^2/2m anyway
    std::function<RVec(double)> V;
    RVec T_p; // Shifted momentum space kinetic operator
    // Eigen::SparseMatrix<double> T; // position space kinetic operator

    HamiltonianFn(HilbertSpace &hs, std::function<RVec(double)> V) : hs(hs), V(V) //,T(hs.T())
    {
        auto dx = hs.dx();
        auto size = hs.dim();
        // RVec temp = RVec::LinSpaced(size,-PI/dx,PI/dx);

        // S_INFO("dp: ",2*PI/(size*dx)," vs ",temp[1]-temp[0]);
        p = RVec::Zero(size);
        // p.array().bottomRows(size/2) = temp.array().topRows(size/2);
        // p.array().topRows(size/2) = temp.array().bottomRows(size/2);

        auto dp = 2*PI/dx/size;
        for (auto i = 0; i<size/2;i++){p[i]=dp*i;}
        for (auto i = size/2; i<size;i++){p[i]=dp*(-size+i);}

        T_p = p.cwisePow(2)/2;
    }

    HamiltonianFn(HilbertSpace &hs) : HamiltonianFn(hs, [dim = hs.dim()](double u){return RVec::Zero(dim);}) {}

    Hamiltonian operator()(double u){return Hamiltonian(hs, V(u));}
};

#endif // SEAHORSE_HAMILTONIAN