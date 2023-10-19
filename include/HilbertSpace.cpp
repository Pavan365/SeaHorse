#ifndef SEAHORSE_HILBERTSPACE
#define SEAHORSE_HILBERTSPACE

#include "Vectors.cpp"
#include <eigen/Eigen/Core>
#include <eigen/Eigen/Dense>



class HilbertSpace
{
    private:
    // Spatial grid
    const RVec _x;
    const double _dx;
    const RVec _p;
    const double _dp;
    const int _dim;

    // Kinetic energy operator
    Eigen::SparseMatrix<double> _T;

    public:
    //Parameterized Constructor
    // 2*PI*fftshift(x)/(DX * DIM)
    
    HilbertSpace(int dim, double xmin, double xmax) : _x(Eigen::VectorXd::LinSpaced(dim,xmin,xmax)),_dx(_x[1]-_x[0]), _p(Eigen::VectorXd::LinSpaced(dim,xmin,xmax)),_dp(_p[1]-_p[0]), _dim(dim)
    {
        Eigen::SparseMatrix<double> Laplace(dim,dim);
        Laplace.reserve(Eigen::VectorXi::Constant(dim,3));
        for(auto i=0;i<dim;i++){
            if(i!=0) Laplace.coeffRef(i-1,i)= 1;
            Laplace.coeffRef(i,i) = -2;
            if(i!=dim-1) Laplace.coeffRef(i+1,i)= 1;
        }
        Laplace/= (_dx*_dx); // rescale derivative

        _T = Laplace * (-0.5);// p^2/2m = -(d^2/dx^2)(h_bar)^2/2m  with m=1,h_bar=1
    }

    auto x() const {return _x;}
    auto dx() const {return _dx;}
    auto dim() const {return _dim;}
    auto T() const {return _T;} // Kinetic operator

    HilbertSpace(int dim, double xlim): HilbertSpace(dim,-xlim,xlim){};
};

#endif // SEAHORSE_HILBERTSPACE