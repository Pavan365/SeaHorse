#include <fftw3.h>
#include <eigen/Eigen/Core>
#include <eigen/Eigen/Dense>
#include <eigen/unsupported/Eigen/FFT>

#include <spectra/SymEigsSolver.h>
#include <spectra/MatOp/SparseSymMatProd.h>

#include <iostream>

constexpr double PI = 3.141592653589793116;

// Allows maths style notation
template<typename T> auto cos(T v) {return v.array().cos();}
template<typename T> auto sin(T v) {return v.array().sin();}
template<typename T> auto exp(T v) {return v.array().exp();}
template<typename T, typename T2> auto box(T v, T2 x, double min, double max) {return (min < x.array() && x.array() < max ).select(v,0);}


#ifdef WITHOUT_NUMPY
#include "matplotlib/matplotlibcpp.h"
namespace plt = matplotlibcpp;
void plots(const Eigen::VectorXd& x,const Eigen::VectorXd& y){
    plt::figure();

    plt::plot(x, y, "k", {{"label","V"}});

    plt::xlabel("x (a.u.)");
    plt::ylabel("Energy $(E_r)$");

    plt::legend();

    plt::grid();
    // plt::savefig("output/test.png"); // show the figure instead of saving it

    // plt::show();
}
#else
void plots(const Eigen::VectorXd& x,const Eigen::VectorXd& y){}
#endif

class Stepper {

};

class Hamiltonian;

class HilbertSpace
{
    private:
    // Spatial grid
    Eigen::VectorXd _x;
    double _dx;
    const int _dim;

    // Kinetic energy operator
    Eigen::SparseMatrix<double> _T;

    public:
    //Parameterized Constructor
    HilbertSpace(int dim, double xmin, double xmax) : _x(Eigen::VectorXd::LinSpaced(dim,xmin,xmax)),_dx(_x(1)-_x(0)), _dim(dim)
    {
        //auto temp = Eigen::MatrixXd(dim,dim);
        Eigen::SparseMatrix<double> Laplace(dim,dim);
        Laplace.reserve(Eigen::VectorXi::Constant(dim,3));
        for(auto i=0;i<dim;i++){
            if(i!=0) Laplace.coeffRef(i-1,i)= 1;
            Laplace.coeffRef(i,i) = -2;
            if(i!=dim-1) Laplace.coeffRef(i+1,i)= 1;
        }
        Laplace/= (_dx*_dx); // rescale derivative

        _T = Laplace * (-0.5);// p^2/2m = -(D/dx)^2/2m  with m=1
    }

    auto x() const {return _x.array();}
    auto dx() const {return _dx;}
    auto dim() const {return _dim;}
    auto T() const {return _T;}

    HilbertSpace(int dim, double xlim): HilbertSpace(dim,-xlim,xlim){};
};

class HamiltonianEvaluated
{
    public:
    Eigen::SparseMatrix<double> H;

    // Components of H
    Eigen::VectorXd V;
    Eigen::SparseMatrix<double> T;

    HamiltonianEvaluated(Eigen::SparseMatrix<double> T, Eigen::VectorXd V): V(V),T(T)  {
        H = T + V.asDiagonal().toDenseMatrix().sparseView();
    }

    Eigen::VectorXd eigenvalues(){
        // Finds all eigenvalues for the hamiltonian
        auto temp = T.toDense();

        Eigen::VectorXd diag = temp.diagonal().array() + V.array();
        Eigen::VectorXd subdiag = temp.diagonal(1);

        Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigensolver(diag.size());
        eigensolver.computeFromTridiagonal(diag,subdiag,Eigen::EigenvaluesOnly);

        if (eigensolver.info() != Eigen::Success) std::cout<<"Failed to calculate full spectrum";

        return eigensolver.eigenvalues();
    }

    Eigen::MatrixXd spectrum(int num){
        // 'Spectra' implementation finds first 'num' eigenvalues/eigenvectors
        //  Columns are are eigenvalue followed by full eigenvector

        // Construct matrix operation object using the wrapper class SparseSymMatProd
        Spectra::SparseSymMatProd<double> op(H);
        // Construct eigen solver object, requesting the largest num eigenvalues
        Spectra::SymEigsSolver<Spectra::SparseSymMatProd<double>> eigs(op, std::min(num,(int)H.cols()-1), std::min(4*num,(int)H.cols()));

        // Initialize and compute
        eigs.init();
        int nconv = eigs.compute(Spectra::SortRule::SmallestMagn);

        // Retrieve results
        Eigen::MatrixXd evvalues;
        if(eigs.info() == Spectra::CompInfo::Successful){
            Eigen::VectorXd evalues = eigs.eigenvalues();
            evvalues = eigs.eigenvectors();

            evvalues.conservativeResize(evvalues.rows()+1, evvalues.cols());
            evvalues.row(evvalues.rows()-1) = evalues;
        }

        else if(eigs.info() == Spectra::CompInfo::NotConverging) {std::cout<<"NotConverging, only "<<nconv<<" eigenvalues"<<std::endl;}
        else if(eigs.info() == Spectra::CompInfo::NumericalIssue) {std::cout<<"NumericalIssue"<<std::endl;}
        else if(eigs.info() == Spectra::CompInfo::NotComputed) {std::cout<<"NotComputed"<<std::endl;}

        return evvalues.rowwise().reverse();
    }

    Eigen::MatrixXd eigenvectors(int num){
        // First 'num' eigenvectors without eigenvalues
        auto eigs = spectrum(num);
        eigs.conservativeResize(eigs.rows()-1, eigs.cols());
        return eigs;
    }
};

class Hamiltonian
{
    public:
    HilbertSpace& hs;
    std::function<Eigen::VectorXd(double)> V;

    Hamiltonian(HilbertSpace& hs) : hs(hs){
        V = [x=hs.x()](double u){return Eigen::VectorXd (x * 0);};
    }

    HamiltonianEvaluated operator()(double u){return HamiltonianEvaluated(hs.T(),V(u));}

    void setV(std::function<Eigen::VectorXd(double)> V0){V = V0;}

};