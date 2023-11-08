#include "libs/seahorse/include/Hamiltonian.h"

// Actual construction from result
Spectrum::Spectrum(const RVec& eigs, const Eigen::MatrixXd& eigvs)
    : eigenvalues(eigs)
    , numEigs(eigs.size())
    , eigenvectors(eigvs)
    , numEigvs(eigvs.cols())
{
}

// Return the eigenvalue (internal calls from Hamiltonian only)
double Spectrum::eigenvalue(int i)
{
    if (i < 0 || i > numEigs - 1) {
        S_ERROR("Invalid eigenvalue");
        return 0;
    };
    return eigenvalues[i];
}
// Return the eigenvector (internal calls from Hamiltonian only)
const RVec Spectrum::eigenvector(int i)
{
    if (i < 0 || i > numEigvs - 1) {
        S_ERROR("Invalid eigenvector");
        return RVec::Zero(2);
    }
    return eigenvectors.col(i);
}

// Constructor
Hamiltonian::Hamiltonian(HilbertSpace& hs, const RVec& V)
    : V(V)
{
    H = hs.T();
    H.diagonal() += V;
}

// Operator for getting/calculating eigenvectors
RVec Hamiltonian::operator[](int i)
{
    if (i >= spectrum.numEigvs) // We have't already calculated enough states
    {
        // NB: i+1 as we zero index so the 0th is calcSpectrum(1)
        calcSpectrum(i + 1);
    }
    return spectrum.eigenvector(i);
}

// Calculation of only the eigenvalues WARNING THIS CAN BE VERY COSTLY FOR LARGE DIMENSION
double Hamiltonian::eigenvalue(int i)
{
    // Might as well calculate all as its fast and we then cache them
    if (i >= spectrum.numEigs) {
        S_INFO("Calculating all eigenvalues (This may be very slow for large dimensions)");
        calcEigenvalues();
    }
    return spectrum.eigenvalue(i);
}

// Calculate all eigenvalues
void Hamiltonian::calcEigenvalues()
{
    Timer timer;
    timer.Start();
    // Finds all eigenvalues for the hamiltonian
    // We can't use tridiag solver as we use a 4th order d2/dx2 approx
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver;

    eigensolver.compute(H, Eigen::EigenvaluesOnly);

    if (eigensolver.info() != Eigen::Success)
        std::cerr << "Failed to calculate full spectrum";
    spectrum.eigenvalues = eigensolver.eigenvalues();
    spectrum.numEigs = spectrum.eigenvalues.size();

    timer.Stop("calcEigenvalues");
}

// Calculating the full spectrum of the matrix up to state `num`
void Hamiltonian::calcSpectrum(int num, bool looped, double smallest_eigenvalue)
{
    // 'Spectra' implementation finds first 'num' eigenvalues/eigenvectors of a SPARSE SYMMETRIC matrix
    auto nev = std::min(std::max(num, 4), (int)H.cols() - 1);
    auto ncv = std::min(4 * nev, (int)H.cols());

    // Construct matrix operation object using the wrapper class SparseSymShiftSolve
    Spectra::SparseSymShiftSolve<double> op(H);
    // estimate the smallest eigenvalue by the smallest element in the matrix
    if (smallest_eigenvalue == 0) {
        smallest_eigenvalue = H.coeffs().minCoeff();
        smallest_eigenvalue = smallest_eigenvalue > 0 ? 0 : smallest_eigenvalue * 2;
        S_INFO("Minimum eigenvalue estimated from the Hamiltonian minCoeff. Please provide a close estimate (that MUST be lower than the actual value) to speedup calculation. We guessed ", smallest_eigenvalue);
    }
    Spectra::SymEigsShiftSolver<Spectra::SparseSymShiftSolve<double>> eigs(op, nev, ncv, smallest_eigenvalue);
    eigs.init();
    int nconv = eigs.compute(Spectra::SortRule::LargestAlge);

    // Retrieve results
    if (eigs.info() == Spectra::CompInfo::NotConverging) {
        S_ERROR("calcSpectrum: Not Converging (nev:", nev, ",ncv:", ncv, ") only ", nconv, " eigenvalues");
    } else if (eigs.info() == Spectra::CompInfo::NumericalIssue) {
        S_ERROR("calcSpectrum: Spectra Numerical Issue");
    } else if (eigs.info() == Spectra::CompInfo::NotComputed) {
        S_ERROR("calcSpectrum: Spectra refused to compute spectrum?");
    }
    // If we got values then update the spectrum
    if (eigs.info() == Spectra::CompInfo::Successful || (eigs.info() == Spectra::CompInfo::NotConverging && nconv != 0)) {
        Eigen::VectorXd eig = eigs.eigenvalues().reverse();
        Eigen::MatrixXd eigv = eigs.eigenvectors().rowwise().reverse();

        if (eig.size() > spectrum.numEigs) {
            spectrum.eigenvalues = eig;
            spectrum.numEigs = eig.size();
        }
        if (eigv.cols() > spectrum.numEigvs) {
            spectrum.eigenvectors = eigv;
            spectrum.numEigvs = eigv.cols();
        }
    } else if (looped == false) // It failed to get any eigenvectors so try again with more lax numbers
    {
        S_INFO("Recalculating eigenspectrum with ", 2 * nev + 5, " states instead of ", nev);
        calcSpectrum(2 * nev + 5, true);
    }
    return;
}

// Constructor
HamiltonianFn::HamiltonianFn(HilbertSpace& hs, std::function<RVec(double)> V)
    : hs(hs)
    , V(V)
{
    auto dx = hs.dx();
    auto size = hs.dim();
    p = RVec::Zero(size);

    auto dp = 2 * PI / dx / size;
    for (auto i = 0; i < size / 2; i++) {
        p[i] = dp * i;
    }
    for (auto i = size / 2; i < size; i++) {
        p[i] = dp * (-size + i);
    }

    T_p = p.cwisePow(2) / 2;
}

// Construct with no potential
HamiltonianFn::HamiltonianFn(HilbertSpace& hs)
    : HamiltonianFn(hs, [dim = hs.dim()](double u) { return RVec::Zero(dim); })
{
}

// Evaluate HFn to H
Hamiltonian HamiltonianFn::operator()(double u) { return Hamiltonian(hs, V(u)); }
