#include "libs/seahorse/include/HilbertSpace.h"

// Constructors
HilbertSpace::HilbertSpace(int dim, double xmin, double xmax)
    : m_x(Eigen::VectorXd::LinSpaced(dim, xmin, xmax))
    , m_dx(m_x[1] - m_x[0])
    , m_dim(dim)
{
    if (dim % 2 != 0 || dim < 5)
        S_FATAL("Dimension of Hilbert Space should be a multiple of 2 but is ", dim);
    // 4th order central difference approx for d2/dx2
    Eigen::SparseMatrix<double> Laplace(dim, dim);
    Laplace.reserve(Eigen::VectorXi::Constant(dim, 5));
    for (auto i = 0; i < dim; i++) {
        if (i > 1) {
            Laplace.coeffRef(i - 2, i) = -1;
        } else {
            Laplace.coeffRef(i - 2 + dim, i) = -1;
        }
        if (i > 0) {
            Laplace.coeffRef(i - 1, i) = 16;
        } else {
            Laplace.coeffRef(i - 1 + dim, i) = 16;
        }
        Laplace.coeffRef(i, i) = -30;
        if (i < dim - 1) {
            Laplace.coeffRef(i + 1, i) = 16;
        } else {
            Laplace.coeffRef(i + 1 - dim, i) = 16;
        }
        if (i < dim - 2) {
            Laplace.coeffRef(i + 2, i) = -1;
        } else {
            Laplace.coeffRef(i + 2 - dim, i) = -1;
        }
    }
    Laplace /= ((m_dx * m_dx) * 12); // rescale derivative
    m_T = Laplace * (-0.5); // p^2/2m = -(d^2/dx^2)(h_bar)^2/2m  with m=1,h_bar=1
}

HilbertSpace::HilbertSpace(int dim, double xlim)
    : HilbertSpace(dim, -xlim, xlim) {};

RVec HilbertSpace::x() const { return m_x; }
double HilbertSpace::dx() const { return m_dx; }
int HilbertSpace::dim() const { return m_dim; }
inline Eigen::SparseMatrix<double> HilbertSpace::T() const { return m_T; } // Kinetic operator
