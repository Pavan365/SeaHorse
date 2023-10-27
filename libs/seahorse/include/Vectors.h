/*
    NOTE: Because we need to use this as both declaration and definitions everything must
    be static/inline to prevent the symbols being duplicated when linking againts libseahorse.
*/

#pragma once
#include "seahorse/include/Globals.h"

using RVec = Eigen::VectorXd;
using CVec = Eigen::VectorXcd;

// Allow maths style notation on real/complex Vecs + Scalars
// We promote the type to complex only where needed
// This is done by casting to the Vector's type when adding doubles but casting the vectors to complex when adding complex
template <typename Derived>
inline const auto operator+(const Eigen::MatrixBase<Derived> &v, double d) {return v.array() + static_cast<typename Derived::Scalar>(d);}
template <typename Derived>
inline const auto operator+(double d, const Eigen::MatrixBase<Derived> &v) {return static_cast<typename Derived::Scalar>(d) + v.array();}
template <typename Derived>
inline const auto operator-(double d, const Eigen::MatrixBase<Derived> &v) {return static_cast<typename Derived::Scalar>(d) - v.array();}
template <typename Derived>
inline const auto operator-(const Eigen::MatrixBase<Derived> &v, double d) {return v.array() - static_cast<typename Derived::Scalar>(d);}
template <typename Derived>
inline const auto operator+(const Eigen::MatrixBase<Derived> &v, std::complex<double> d) {return v.template cast<std::complex<double>>().array() + d;}
template <typename Derived>
inline const auto operator+(std::complex<double> d, const Eigen::MatrixBase<Derived> &v) {return d + v.template cast<std::complex<double>>().array();}
template <typename Derived>
inline const auto operator-(std::complex<double> d, const Eigen::MatrixBase<Derived> &v) {return d - v.template cast<std::complex<double>>().array();}
template <typename Derived>
inline const auto operator-(const Eigen::MatrixBase<Derived> &v, std::complex<double> d) {return v.template cast<std::complex<double>>().array() - d;}


inline const auto exp(const RVec &v);
inline const auto exp(const CVec &v);
inline const auto abs2(const RVec &v);
inline const auto abs2(const CVec &v);

// These only really make sense on the Reals
inline const auto cos(const RVec &v);
inline const auto sin(const RVec &v);
inline const auto box(const RVec &x, double min, double max);
