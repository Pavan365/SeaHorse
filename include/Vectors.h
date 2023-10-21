#include <eigen/Eigen/Core>
#include <complex>
#ifndef SEAHORSE_VECTORS
#define SEAHORSE_VECTORS

struct RVec
{
    Eigen::VectorXd _vec;

    // Constructors
    RVec() : _vec(Eigen::VectorXd()) {}
    RVec(int dim) : _vec(Eigen::VectorXd::Zero(dim)) {}
    RVec(int dim, double fill) : RVec(dim) { _vec.fill(fill); }
    RVec(Eigen::VectorXd v) : _vec(v) {}

    // Useful functions
    const inline auto size() const { return _vec.size(); }
    double &operator[](int i)
    {
        if (i < 0 || i > _vec.size() - 1)
        {
            std::cerr << "Out of Bounds indexing RVec" << std::endl;
        }
        return _vec[i];
    }
    const double operator[](int i) const
    {
        if (i < 0 || i > _vec.size() - 1)
        {
            std::cerr << "Out of Bounds indexing RVec" << std::endl;
        }
        return _vec[i];
    }
    const inline double norm(){return _vec.norm();}

    // I'm not sure this actually works...
    inline const auto operator^(const double d) const { return _vec.cwisePow(d); }

    // Rvec on Rvec operations - if these are auto 
    inline const auto operator+(RVec v) const { return _vec.array() + v._vec.array(); }
    inline const auto operator-(RVec v) const { return _vec.array() + v._vec.array(); }
    inline const auto operator*(RVec v) const { return _vec.array() * v._vec.array(); }
    inline const auto operator/(RVec v) const { return _vec.array() / v._vec.array(); }

    // Casting Eigen's lazy things back to RVec
    template <typename T, typename T2>
    RVec(class Eigen::CwiseUnaryOp<T, T2> v) : RVec(v.eval()) {std::cout<<"\n"<<__PRETTY_FUNCTION__<<"\n"<<std::endl;}
    template <typename T, typename T2, typename T3>
    RVec(class Eigen::CwiseBinaryOp<T, T2, T3> v) : RVec(v.eval()) {std::cout<<"\n"<<__PRETTY_FUNCTION__<<"\n"<<std::endl;}
    template <typename T, typename T2, typename T3, typename T4>
    RVec(class Eigen::CwiseTernaryOp<T, T2, T3, T4> v) : RVec(v.eval()) {std::cout<<"\n"<<__PRETTY_FUNCTION__<<"\n"<<std::endl;}
};

// Eigens lazy operators on RVecs and doubles
inline const auto operator+(const RVec &v, const double d) { return v._vec.array() + d; }
inline const auto operator-(const RVec &v, const double d) { return v._vec.array() - d; }
inline const auto operator*(const RVec &v, const double d) { return v._vec.array() * d; }
inline const auto operator/(const RVec &v, const double d) { return v._vec.array() / d; }
inline const auto operator+(const double d, const RVec &v) { return d + v._vec.array(); }
inline const auto operator-(const double d, const RVec &v) { return d - v._vec.array(); }
inline const auto operator*(const double d, const RVec &v) { return d * v._vec.array(); }
inline const auto operator/(const double d, const RVec &v) { return d / v._vec.array(); }

// easy streaming of RVecs
std::ostream &operator<<(std::ostream &os, const RVec &v) { return os << v._vec; }

// Probably inherit from Vec -> both RVec and CVec
// could be some sort of template thingy? Vec<double>==RVec, Vec<std::complex<double>>==CVec?
// most functions will be the same but may make promoting to CVec hard?
// can make: directly/complex+-*/RVec
// struct CVec
// {
//     Eigen::VectorXcd _vec;
// };

// Allows maths style notation on vectors
inline const auto cos(const RVec &v) { return v._vec.array().cos(); }
inline const auto sin(const RVec &v) { return v._vec.array().sin(); }
inline const auto exp(const RVec &v) { return v._vec.array().exp(); }
inline const auto abs2(const RVec &v) { return v._vec.array().abs2(); }

const auto box(const RVec &x, double min, double max)
{
    return (x._vec.array().cwiseLess(max).cast<double>() + x._vec.array().cwiseGreater(min).cast<double>()) - 1;
}

#endif // SEAHORSE_VECTORS