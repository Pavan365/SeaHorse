#pragma once
#include <iostream>
#include <complex>
#include <eigen/Eigen/Core>
#include <eigen/Eigen/Dense>
#include <Eigen/SparseCore>

static constexpr double PI = 3.141592653589793116;
using namespace std::complex_literals;

using RVec = Eigen::VectorXd;
using CVec = Eigen::VectorXcd;

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define S_INFO(...) S_LOG_IMPL("\033[92m[INFO]\033[0m ", __LINE__, __FILE__, __VA_ARGS__)
#define S_ERROR(...) S_LOG_IMPL("\033[91m[ERROR]\033[0m", __LINE__, __FILE__, __VA_ARGS__)
#define S_FATAL(...) {S_LOG_IMPL("\033[91m[FATAL]", __LINE__, __FILE__, __VA_ARGS__); exit(EXIT_FAILURE);}

template <typename ...Args>
void S_LOG_IMPL(const char* type, int line, const char* fileName, Args&& ...args) {
    std::ostringstream stream;
    stream << type << " " <<fileName << ":" << line << " - ";
    (stream << ... << std::forward<Args>(args)) << '\n';

    printf(stream.str().c_str());
}
