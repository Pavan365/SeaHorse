#include <eigen/Eigen/Core>
#include <complex>
#include "Globals.h"
#ifndef SEAHORSE_VECTORS
#define SEAHORSE_VECTORS

using RVec = Eigen::VectorXd;
using CVec = Eigen::VectorXcd;

// Allow maths style notation on RVecs + doubles
template <typename Derived>
inline const auto operator+(const Eigen::MatrixBase<Derived> &v, double d) {S_INFO("RVec+d");return v.array() + static_cast<typename Derived::Scalar>(d);}
template <typename Derived>
inline const auto operator+(double d, const Eigen::MatrixBase<Derived> &v) {S_INFO("d+RVec");return static_cast<typename Derived::Scalar>(d) + v.array();}
template <typename Derived>
inline const auto operator-(double d, const Eigen::MatrixBase<Derived> &v) {S_INFO("d+RVec");return static_cast<typename Derived::Scalar>(d) - v.array();}
template <typename Derived>
inline const auto operator-(const Eigen::MatrixBase<Derived> &v, double d) {S_INFO("d+RVec");return v.array() - static_cast<typename Derived::Scalar>(d);}


inline const auto exp(const RVec &v) { return v.array().exp(); }
inline const auto exp(const CVec &v) { return v.array().exp(); }
inline const auto abs2(const RVec &v) { return v.array().abs2(); }
inline const auto abs2(const CVec &v) { return v.array().abs2(); }

// These only really make sense on the Reals
inline const auto cos(const RVec &v) { return v.array().cos(); }
inline const auto sin(const RVec &v) { return v.array().sin(); }
const auto box(const RVec &x, double min, double max)
{
    return (x.array().cwiseLess(max).cast<double>() + x.array().cwiseGreater(min).cast<double>()) - 1;
}


#endif // SEAHORSE_VECTORS