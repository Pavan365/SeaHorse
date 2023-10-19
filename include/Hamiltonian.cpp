#ifndef SEAHORSE_HAMILTONIAN
#define SEAHORSE_HAMILTONIAN
#include "Vectors.cpp"
#include <iostream>
#include <eigen/Eigen/Core>

class Spectrum {
    public:
    RVec eigenvalues;
    Eigen::MatrixXd eigenvectors;
    int num;

    Spectrum(RVec eigs, Eigen::MatrixXd eigvs) : eigenvalues(eigs), eigenvectors(eigvs), num(eigs.size()) {}
    
    double eigenvalue(int i)
    {
        if (i <0 || i > num-1) std::cerr<<"Invalid eigenvalue"<<std::endl;
        return eigenvalues[num-i-1];
    }
    RVec eigenvector(int i)
    {
        if (i <0 || i > num-1) std::cerr<<"Invalid eigenvector"<<std::endl;
        return (Eigen::VectorXd)eigenvectors.col(num-i-1);
    }
};


class Hamiltonian
// NB This is a spatial Hamiltonian (real valued potentials)
{
    public:
    Eigen::SparseMatrix<double> H;

    // Components of H
    RVec V;
    Eigen::SparseMatrix<double> T;

    Hamiltonian(Eigen::SparseMatrix<double> T, RVec V): V(V),T(T)  {
        H = T + V._vec.asDiagonal().toDenseMatrix().sparseView();
    }

    RVec eigenvalues(){
        // Finds all eigenvalues for the hamiltonian
        auto temp = T.toDense();

        Eigen::VectorXd diag =  temp.diagonal().array() + V._vec.array();
        Eigen::VectorXd subdiag = temp.diagonal(1);

        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(diag.size());
        eigensolver.computeFromTridiagonal(diag,subdiag,Eigen::EigenvaluesOnly);

        if (eigensolver.info() != Eigen::Success) std::cout<<"Failed to calculate full spectrum";

        return eigensolver.eigenvalues();
    }

   Spectrum spectrum(int num){
        // 'Spectra' implementation finds first 'num' eigenvalues/eigenvectors

        auto nev = std::max(std::min(num,(int)H.cols()-1),1);
        auto ncv = std::min(3*num+10,(int)H.cols());
        std::cout<<"num: "<<num<<", nev: "<<nev<<", ncv:"<<ncv<<std::endl;
        // Construct matrix operation object using the wrapper class SparseSymMatProd
        Spectra::SparseSymMatProd<double> op(H);
        // Construct eigen solver object, requesting the smallest 'num' eigenvalues
        Spectra::SymEigsSolver<Spectra::SparseSymMatProd<double>> eigs(op, nev, ncv);

        // Initialize and compute
        eigs.init();
        int nconv = eigs.compute(Spectra::SortRule::SmallestMagn);

        // Retrieve results
        if(eigs.info() == Spectra::CompInfo::Successful){
            return Spectrum(eigs.eigenvalues(),eigs.eigenvectors());
        }

        else if(eigs.info() == Spectra::CompInfo::NotConverging) {std::cerr<<"Spectrum NotConverging, only "<<nconv<<" eigenvalues"<<std::endl;}
        else if(eigs.info() == Spectra::CompInfo::NumericalIssue) {std::cerr<<"Spectrum NumericalIssue"<<std::endl;}
        else if(eigs.info() == Spectra::CompInfo::NotComputed) {std::cerr<<"Spectrum NotComputed"<<std::endl;}

        // Return empty Spectrum
        return Spectrum(RVec(0),{});
    }

};

class HamiltonianFn
{
    public:
    HilbertSpace& hs;
    std::function<RVec(double)> V;

    HamiltonianFn(HilbertSpace& hs) : hs(hs)
    {
        V = [dim=hs.dim()](double u){
            return RVec(dim,0);
        };
    }

    Hamiltonian operator()(double u)
    {
        return Hamiltonian(hs.T(),V(u));
    }

    void setV(std::function<RVec(double)> V0)
    {
        V = V0;
    }
};

#endif // SEAHORSE_HAMILTONIAN