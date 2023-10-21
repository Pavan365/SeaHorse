#include <eigen/Eigen/Core>
#include <complex>
#ifndef SEAHORSE_VECTORS
#define SEAHORSE_VECTORS

template<typename ScalarType>
struct Vec
{
    using VecType = Eigen::Matrix<ScalarType, -1, 1>;

    public:
    VecType _vec;

    // Constructors
    Vec() : _vec(VecType()) {} // Empty
    Vec(int size) : _vec(Eigen::VectorXd::Zero(size)) {} // Zeros
    Vec(int size, ScalarType fill) : Vec(size) { _vec.fill(fill); } // Filled
    Vec(VecType v) : _vec(v) {} // Copied

    // Useful functions
    const inline int size() const { return _vec.size(); }
    const ScalarType operator[](int i) const
    {
        if (i < 0 || i > _vec.size() - 1)
        {
            std::cerr << "Out of Bounds indexing Vec" << std::endl;
        }
        return _vec[i];
    }
    const inline double norm(){return _vec.norm();}
    const inline double conj(){return _vec.conj();}

    // Vec on Vec operations - same type
    inline const auto operator+(Vec v) const { return _vec.array() + v._vec.array(); }
    inline const auto operator-(Vec v) const { return _vec.array() + v._vec.array(); }
    inline const auto operator*(Vec v) const { return _vec.array() * v._vec.array(); }
    inline const auto operator/(Vec v) const { return _vec.array() / v._vec.array(); }

    // Vec on double
    inline const auto operator+(const double d) { return _vec.array() + d; }
    inline const auto operator-(const double d) { return _vec.array() - d; }
    inline const auto operator*(const double d) { return _vec.array() * d; }
    inline const auto operator/(const double d) { return _vec.array() / d; }

    // Vec on complex
    inline const auto operator+(const std::complex<double> d) { return _vec.array() + d; }
    inline const auto operator-(const std::complex<double> d) { return _vec.array() - d; }
    inline const auto operator*(const std::complex<double> d) { return _vec.array() * d; }
    inline const auto operator/(const std::complex<double> d) { return _vec.array() / d; }
};

struct RVec : Vec<double>
{
    // Constructors
    RVec() : Vec(Eigen::VectorXd()) {} // Empty
    RVec(int size) : Vec(Eigen::VectorXd::Zero(size)) {} // Zeros
    RVec(int size, double fill) : Vec(size,fill) {} // Filled
    RVec(Eigen::VectorXd v) : Vec(v) {} // Copied

    // Casts
    // These are specialized to double so will be used with preference
    // They take a lazy eigen expression which is purely real and allow an RVec to be made from it
    template <template<typename> typename T, typename T2>
    RVec(class Eigen::CwiseUnaryOp<T<double>, T2> v) : RVec((Eigen::VectorXd)v) {}
    template <template < typename, typename> typename T, typename T2>
    RVec(class Eigen::CwiseUnaryOp<T<double,double>, T2> v) : RVec((Eigen::VectorXd)v) {}
    template <template < typename, typename, typename> typename T, typename T2>
    RVec(class Eigen::CwiseUnaryOp<T<double,double,double>, T2> v) : RVec((Eigen::VectorXd)v) {}

    template <template<typename> typename T, typename T2, typename T3>
    RVec(class Eigen::CwiseBinaryOp<T<double>, T2, T3> v) : RVec((Eigen::VectorXd)v) {}
    template <template < typename, typename> typename T, typename T2, typename T3>
    RVec(class Eigen::CwiseBinaryOp<T<double,double>, T2, T3> v) : RVec((Eigen::VectorXd)v) {}
    template <template < typename, typename, typename> typename T, typename T2, typename T3>
    RVec(class Eigen::CwiseBinaryOp<T<double,double,double>, T2, T3> v) : RVec((Eigen::VectorXd)v) {}

    template <template<typename> typename T, typename T2, typename T3, typename T4>
    RVec(class Eigen::CwiseTernaryOp<T<double>, T2, T3, T4> v) : RVec((Eigen::VectorXd)v) {}
    template <template < typename, typename> typename T, typename T2, typename T3, typename T4>
    RVec(class Eigen::CwiseTernaryOp<T<double,double>, T2, T3, T4> v) : RVec((Eigen::VectorXd)v) {}
    template <template < typename, typename, typename> typename T, typename T2, typename T3, typename T4>
    RVec(class Eigen::CwiseTernaryOp<T<double,double,double>, T2, T3, T4> v) : RVec((Eigen::VectorXd)v) {}


    // template <typename T, typename T2>
    // explicit RVec(class Eigen::Product<T, T2> v) : RVec((VecType)v) {}
};

struct CVec : Vec<std::complex<double>>
{
    // Constructors
    CVec() : Vec(Eigen::VectorXcd()) {} // Empty
    CVec(int size) : Vec(Eigen::VectorXcd::Zero(size)) {} // Zeros
    CVec(int size, double fill) : Vec(size,fill) {} // Filled
    CVec(Eigen::VectorXcd v) : Vec(v) {} // Copied
    // CVec(Eigen::VectorXd v) : Vec(v) {} // From Eigen
    CVec(RVec v) : Vec(v._vec) {} // From Eigen

    // Casts
    // This uses enable_if to prevent clashing with RVecs when everything is real
    // Casting Eigen's lazy things back to any CVec
    template <template < typename> typename T, typename T2, typename D,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D>>>
    CVec(class Eigen::CwiseUnaryOp<T<D>, T2> v) : CVec((Eigen::VectorXcd)v) {}
    template <template < typename, typename> typename T, typename T2, typename D, typename D2,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D> || std::is_same_v<std::complex<double>, D2>>>
    CVec(class Eigen::CwiseUnaryOp<T<D,D2>, T2> v) : CVec((Eigen::VectorXcd)v) {}
    template <template < typename, typename, typename> typename T, typename T2, typename D, typename D2, typename D3,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D> || std::is_same_v<std::complex<double>, D2> || std::is_same_v<std::complex<double>, D3>>>
    CVec(class Eigen::CwiseUnaryOp<T<D,D2,D3>, T2> v) : CVec((Eigen::VectorXcd)v) {}

    template <template < typename> typename T, typename T2, typename T3, typename D,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D>>>
    CVec(class Eigen::CwiseBinaryOp<T<D>, T2, T3> v) : CVec((Eigen::VectorXcd)v) {}
    template <template < typename, typename> typename T, typename T2, typename T3, typename D, typename D2,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D> || std::is_same_v<std::complex<double>, D2>>>
    CVec(class Eigen::CwiseBinaryOp<T<D,D2>, T2, T3> v) : CVec((Eigen::VectorXcd)v) {}
    template <template < typename, typename, typename> typename T, typename T2, typename T3, typename D, typename D2, typename D3,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D> || std::is_same_v<std::complex<double>, D2> || std::is_same_v<std::complex<double>, D3>>>
    CVec(class Eigen::CwiseBinaryOp<T<D,D2,D3>, T2, T3> v) : CVec((Eigen::VectorXcd)v) {}

    template <template < typename> typename T, typename T2, typename T3, typename T4, typename D,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D>>>
    CVec(class Eigen::CwiseTernaryOp<T<D>, T2, T3, T4> v) : CVec((Eigen::VectorXcd)v) {}
    template <template < typename, typename> typename T, typename T2, typename T3, typename T4, typename D, typename D2,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D> || std::is_same_v<std::complex<double>, D2>>>
    CVec(class Eigen::CwiseTernaryOp<T<D,D2>, T2, T3, T4> v) : CVec((Eigen::VectorXcd)v) {}
    template <template < typename, typename, typename> typename T, typename T2, typename T3, typename T4, typename D, typename D2, typename D3,
      class = std::enable_if_t<std::is_same_v<std::complex<double>, D> || std::is_same_v<std::complex<double>, D2> || std::is_same_v<std::complex<double>, D3>>>
    CVec(class Eigen::CwiseTernaryOp<T<D,D2,D3>, T2, T3, T4> v) : CVec((Eigen::VectorXcd)v) {}
};

// Rightsided operators must be declared external to class
// Eigen's lazy operators on RVecs and doubles
inline const auto operator+(const RVec &v, const double d) { return v._vec.array() + d; }
inline const auto operator-(const RVec &v, const double d) { return v._vec.array() - d; }
inline const auto operator*(const RVec &v, const double d) { return v._vec.array() * d; }
inline const auto operator/(const RVec &v, const double d) { return v._vec.array() / d; }
// Eigen's lazy operators on RVecs and complex
inline const auto operator+(const RVec &v, const std::complex<double> d) { return v._vec.array() + d; }
inline const auto operator-(const RVec &v, const std::complex<double> d) { return v._vec.array() - d; }
inline const auto operator*(const RVec &v, const std::complex<double> d) { return v._vec.array() * d; }
inline const auto operator/(const RVec &v, const std::complex<double> d) { return v._vec.array() / d; }
// Eigen's lazy operators on CVecs and doubles
inline const auto operator+(const CVec &v, const double d) { return v._vec.array() + d; }
inline const auto operator-(const CVec &v, const double d) { return v._vec.array() - d; }
inline const auto operator*(const CVec &v, const double d) { return v._vec.array() * d; }
inline const auto operator/(const CVec &v, const double d) { return v._vec.array() / d; }
// Eigen's lazy operators on CVecs and complex
inline const auto operator+(const CVec &v, const std::complex<double> d) { return v._vec.array() + d; }
inline const auto operator-(const CVec &v, const std::complex<double> d) { return v._vec.array() - d; }
inline const auto operator*(const CVec &v, const std::complex<double> d) { return v._vec.array() * d; }
inline const auto operator/(const CVec &v, const std::complex<double> d) { return v._vec.array() / d; }



// easy streaming of Vecs
template <typename ScalarType>
std::ostream &operator<<(std::ostream &os, const Vec<ScalarType> &v) { return os << v._vec; }


// Allow maths style notation on Vecs
template <typename ScalarType>
inline const auto exp(const Vec<ScalarType> &v) { return v._vec.array().exp(); }
template <typename ScalarType>
inline const auto abs2(const Vec<ScalarType> &v) { return v._vec.array().abs2(); }

// These only really make sense on the Reals
inline const auto cos(const RVec &v) { return v._vec.array().cos(); }
inline const auto sin(const RVec &v) { return v._vec.array().sin(); }
const auto box(const RVec &x, double min, double max)
{
    return (x._vec.array().cwiseLess(max).cast<double>() + x._vec.array().cwiseGreater(min).cast<double>()) - 1;
}




#endif // SEAHORSE_VECTORS