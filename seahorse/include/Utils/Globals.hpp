#pragma once
#include <chrono>
#include <complex>
#include <functional>
#include <iostream>

#define INCBIN_PREFIX
#include "include/Utils/incbin.hpp"
#define INCLUDE_sourceFile INCBIN(sourceFile, __FILE__)

// We have to specifically suppress these warnings because of the Eigen library
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wuse-after-free"
#include <libs/eigen/Eigen/Core>
#include <libs/eigen/Eigen/Dense>
#include <libs/eigen/Eigen/SparseCore>
#pragma GCC diagnostic pop

using RVec = Eigen::VectorXd;
using CVec = Eigen::VectorXcd;
using RMat = Eigen::MatrixXd;
using CMat = Eigen::MatrixXcd;

using namespace std::complex_literals;
#define PI M_PI

#define S_INFO(...) S_INFO_IMPL(__VA_ARGS__);
#define S_ERROR(...) S_ERR_IMPL("\033[91m[ERROR]\033[0m", __LINE__, __FILE__, __VA_ARGS__);
#define S_FATAL(...)                                                    \
    {                                                                   \
        S_ERR_IMPL("\033[91m[FATAL]", __LINE__, __FILE__, __VA_ARGS__); \
        exit(EXIT_FAILURE);                                             \
    };
template <typename... Args>
void S_ERR_IMPL(const char* type, int line, const char* fileName,
    Args&&... args)
{
    std::ostringstream stream;
    stream << type << " " << fileName << ":" << line << " -> ";
    (stream << ... << std::forward<Args>(args)) << '\n';

    std::cout << stream.str() << std::endl;
}
template <typename... Args>
void S_INFO_IMPL(Args&&... args)
{
    std::ostringstream stream;
#ifndef NDEBUG
    stream << "\033[93m[DEBUG MODE]\033[0m ";
#endif
    stream << "\033[92m[INFO] \033[0m";
    (stream << ... << std::forward<Args>(args)) << '\n';

    std::cout << stream.str() << std::endl;
}