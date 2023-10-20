#ifndef SEAHORSE_HILBERTSPACE
#define SEAHORSE_HILBERTSPACE
#include <iostream>
#include "Vectors.h"
#include <eigen/Eigen/Core>
#include <eigen/Eigen/Dense>
#include <Eigen/SparseCore>

class HilbertSpace
{
private:
    // Spatial grid
    const RVec _x;
    const double _dx;
    const int _dim;

    // Kinetic energy operator
    Eigen::SparseMatrix<double> _T;

public:
    //  2*PI*fftshift(x)/(DX * DIM)

    // Constructors
    HilbertSpace(int dim, double xmin, double xmax) : _x(Eigen::VectorXd::LinSpaced(dim, xmin, xmax)), _dx(_x[1] - _x[0]), _dim(dim)
    {
        Eigen::SparseMatrix<double> Laplace(dim, dim);
        Laplace.reserve(Eigen::VectorXi::Constant(dim, 3));
        for (auto i = 0; i < dim; i++)
        {
            if (i != 0)
                Laplace.coeffRef(i - 1, i) = 1;
            Laplace.coeffRef(i, i) = -2;
            if (i != dim - 1)
                Laplace.coeffRef(i + 1, i) = 1;
        }
        Laplace /= (_dx * _dx); // rescale derivative

        _T = Laplace * (-0.5); // p^2/2m = -(d^2/dx^2)(h_bar)^2/2m  with m=1,h_bar=1
    }
    HilbertSpace(int dim, double xlim) : HilbertSpace(dim, -xlim, xlim){};

    auto x() const { return _x; }
    auto dx() const { return _dx; }
    auto dim() const { return _dim; }
    auto T() const { return _T; } // Kinetic operator

};

#endif // SEAHORSE_HILBERTSPACE