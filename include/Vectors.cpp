#include <eigen/Eigen/Core>
#ifndef SEAHORSE_VECTORS
#define SEAHORSE_VECTORS

struct RVec
{
    Eigen::VectorXd _vec;

    RVec(int dim) : _vec(Eigen::VectorXd::Zero(dim)) {}
    RVec(int dim, double fill) : RVec(dim) {_vec.fill(fill);}
    RVec(Eigen::VectorXd v) : _vec(v) {}

    const inline auto size() const {return _vec.size();}
    
    // Indexing into vector
    double &operator[] (int i)
    {
        if (i<0 || i>_vec.size()-1) {std::cerr<<"Out of Bounds indexing RVec"<<std::endl;}
        return _vec[i];
    }
    const double operator[] (int i) const
    {
        if (i<0 || i>_vec.size()-1) {std::cerr<<"Out of Bounds indexing RVec"<<std::endl;}
        return _vec[i];
    }


    // Allows operations on the vector
    // NB IDEALLY WE WOULD USE THE EIGEN LAZY CALC STUFF BUT I CANT MAKE IT WORK FULLY
    inline const RVec operator-(const double d) const {return (_vec.array() - d);}
    inline const RVec operator+(const double d) const {return _vec.array() + d;}
    inline const RVec operator*(const double d) const {return _vec.array() * d;}
    inline const RVec operator/(const double d) const {return _vec.array() / d;}
    inline const RVec operator^(const double d) const {return _vec.cwisePow(d);}
    inline const RVec operator*(RVec v) const {return _vec.array() * v._vec.array();}
    // Allows the results to be cast back to RVec
    template <typename T, typename T2>
    RVec(class Eigen::CwiseUnaryOp<T, T2> v) : RVec(((Eigen::VectorXd) v)){}
    template <typename T, typename T2, typename T3>
    RVec(class Eigen::CwiseBinaryOp<T, T2, T3> v) : RVec(((Eigen::VectorXd) v)){}
    template <typename T, typename T2, typename T3, typename T4>
    RVec(class Eigen::CwiseTernaryOp<T, T2, T3, T4> v) : RVec(((Eigen::VectorXd) v)){}
};
// Left sided ops
inline const RVec operator-(const double d, const RVec& v) {return (d - v._vec.array());}
inline const RVec operator+(const double d, const RVec& v) {return v._vec.array() + d;}
inline const RVec operator*(const double d, const RVec& v) {return v._vec.array() * d;}
inline const RVec operator/(const double d, const RVec& v) {return d / v._vec.array();}
std::ostream& operator<<(std::ostream& os, const RVec& v) {return os << v._vec;}

// struct RMat
// {
//     Eigen::MatrixXd _mat;
// };

// Allows maths style notation on vectors

inline const RVec cos(const RVec v) {return v._vec.array().cos();}
inline const RVec sin(const RVec& v) {return v._vec.array().sin();}
inline const RVec exp(const RVec& v) {return v._vec.array().exp();}
const RVec box(const RVec& x, double min, double max) {return (min < x._vec.array() && x._vec.array() < max ).select(Eigen::VectorXd::Ones(x.size()),0);}


#endif // SEAHORSE_VECTORS