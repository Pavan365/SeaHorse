#pragma once

#include "seahorse/include/Vectors.h"

void test(int i);

class HilbertSpace
{
private:
    // Spatial grid
    const RVec m_x;
    const double m_dx = 0;
    const int m_dim = 0;

    // Kinetic energy operator
    Eigen::SparseMatrix<double> m_T;

public:
    // Constructors
    HilbertSpace(int dim, double xmin, double xmax);
    HilbertSpace(int dim, double xlim);

    RVec x() const;
    double dx() const;
    int dim() const;
    inline Eigen::SparseMatrix<double> T() const; // Kinetic operator

};
