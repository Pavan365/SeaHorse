#pragma once
#include <complex>
#include <iostream>
#include <libs/eigen/Eigen/Core>
#include <libs/eigen/Eigen/Dense>
#include <libs/eigen/Eigen/SparseCore>

using RVec = Eigen::VectorXd;
using CVec = Eigen::VectorXcd;
using RMat = Eigen::MatrixXd;
using CMat = Eigen::MatrixXcd;

using namespace std::complex_literals;

#define S_INFO(...) S_LOG_IMPL("\033[92m[INFO]\033[0m ", __LINE__, __FILE__, __VA_ARGS__)
#define S_ERROR(...) S_LOG_IMPL("\033[91m[ERROR]\033[0m", __LINE__, __FILE__, __VA_ARGS__)
#define S_FATAL(...)                                                    \
    {                                                                   \
        S_LOG_IMPL("\033[91m[FATAL]", __LINE__, __FILE__, __VA_ARGS__); \
        exit(EXIT_FAILURE);                                             \
    }

// static constexpr double PI = 3.141592653589793116;
#define PI M_PI

template <typename... Args>
void S_LOG_IMPL(const char* type, int line, const char* fileName,
    Args&&... args)
{
    std::ostringstream stream;
    stream << type << " " << fileName << ":" << line << " -> ";
    (stream << ... << std::forward<Args>(args)) << '\n';

    printf(stream.str().c_str());
}

inline std::string ordinal(int n)
{
    static const char suffixes[][3] = { "th", "st", "nd", "rd" };
    auto ord = n % 100;
    if (ord / 10 == 1) {
        ord = 0;
    }
    ord = ord % 10;
    if (ord > 3) {
        ord = 0;
    }
    return std::to_string(n) + suffixes[ord];
}

// Global timer
class Timer {
private:
    std::chrono::system_clock::time_point start;

public:
    Timer()
        : start(std::chrono::system_clock::now()) {};
    void Start() { start = std::chrono::system_clock::now(); };
    void Stop(std::string msg = "")
    {
        S_INFO(Elapsed(), " seconds, ", msg);
    };
    double Elapsed()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - start).count() / 1e6;
    }
};

// void GenerateFontFile(std::string fontFile = "resources/UbuntuMonoBold.ttf")
// {
//     // Pull in file
//     unsigned int dataSize = 0;
//     unsigned char *fileData = LoadFileData(fontFile.c_str(), &dataSize);
//     // Redirect stdout to the new file
//     FILE *saved = stdout;
//     stdout = fopen((fontFile + ".h").c_str(), "a");

//     // Write data to file
//     printf("unsigned char text_txt_data[] = {");
//     for (auto i = 0; i < dataSize; i++)
//     {
//         if (fileData[i] == 0)
//         {
//             printf("0x00,");
//         }
//         else
//         {
//             printf("%#04x,", fileData[i]);
//         }
//     }
//     printf("};");

//     // Replace stdout
//     fclose(stdout);
//     stdout = saved;
// }