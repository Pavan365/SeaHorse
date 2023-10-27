#include "seahorse/include/Hamiltonian.h"
#include "seahorse/include/HilbertSpace.h"


// Actual construction from result
Spectrum::Spectrum(RVec eigs, Eigen::MatrixXd eigvs) : eigenvalues(eigs), numEigs(eigs.size()), eigenvectors(eigvs), numEigvs(eigvs.cols()) {}

// Return the eigenvalue (internal calls from Hamiltonian only)
double Spectrum::eigenvalue(int i)
{
    if (i < 0 || i > numEigs - 1) {S_ERROR("Invalid eigenvalue");return 0;};
    return eigenvalues[i];
}
// Return the eigenvector (internal calls from Hamiltonian only)
const RVec Spectrum::eigenvector(int i)
{
    if (i < 0 || i > numEigvs - 1) {S_ERROR("Invalid eigenvector");return RVec::Zero(2);}
    return eigenvectors.col(i);
}


// Constructor
Hamiltonian::Hamiltonian(HilbertSpace &hs, RVec V) : V(V) 
{
    H = hs.T();
    H.diagonal()+=V;
}

// Operator for getting/calculating eigenvectors
RVec Hamiltonian::operator[](int i){
    if (i>=spectrum.numEigvs) // We have't already calculated enough states
    {
        // NB: i+1 as we zero index so the 0th is calcSpectrum(1)
        calcSpectrum(i+1);
    }
    return spectrum.eigenvector(i);
}

// Calculation of only the eigenvalues
double Hamiltonian::eigenvalue(int i){
    // Might as well calculate all as its fast and we then cache them
    if (i>=spectrum.numEigs)
    {
        calcEigenvalues();
    }
    return spectrum.eigenvalue(i);
}

// Actually calculate Eigenvalues efficiently
void Hamiltonian::calcEigenvalues()
{
    // Finds all eigenvalues for the hamiltonian
    // We can't use tridiag solver as we use a 4th order d2/dx2 approx
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(H.size());

    eigensolver.compute(H,Eigen::EigenvaluesOnly);

    if (eigensolver.info() != Eigen::Success) std::cerr << "Failed to calculate full spectrum";

    spectrum.eigenvalues = RVec(eigensolver.eigenvalues());
    spectrum.numEigs = spectrum.eigenvalues.size();
}

// Calculating the full spectrum of the matrix up to state `num`
void Hamiltonian::calcSpectrum(int num, bool looped)
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


// Constructor
HamiltonianFn::HamiltonianFn(HilbertSpace &hs, std::function<RVec(double)> V) : hs(hs), V(V)
{
    auto dx = hs.dx();
    auto size = hs.dim();
    p = RVec::Zero(size);

    auto dp = 2*PI/dx/size;
    for (auto i = 0; i<size/2;i++){p[i]=dp*i;}
    for (auto i = size/2; i<size;i++){p[i]=dp*(-size+i);}

    T_p = p.cwisePow(2)/2;
}

// Construct with no potential
HamiltonianFn::HamiltonianFn(HilbertSpace &hs) : HamiltonianFn(hs, [dim = hs.dim()](double u){return RVec::Zero(dim);}) {}

// Evaluate HFn to H
Hamiltonian HamiltonianFn::operator()(double u){return Hamiltonian(hs, V(u));}

