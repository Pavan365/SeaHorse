#pragma once
#include <iostream>
#include <sstream>

#define S_INFO(...) S_INFO_IMPL(__VA_ARGS__);
#define S_ERROR(...) S_ERR_IMPL("\033[1;91m[ERROR]\033[1;0m", __LINE__, __FILE__, __VA_ARGS__);
#define S_FATAL(...)                                                    \
    {                                                                   \
        S_ERR_IMPL("\033[1;91m[FATAL]", __LINE__, __FILE__, __VA_ARGS__); \
        exit(EXIT_FAILURE);                                             \
    };
template <typename... Args>
void S_ERR_IMPL(const char* type, int line, const char* fileName,
    Args&&... args)
{
    std::ostringstream stream;
    stream << type << " " << fileName << ":" << line << " -> ";
    (stream << ... << std::forward<Args>(args));

    std::cout << stream.str() << std::endl;
}
template <typename... Args>
void S_INFO_IMPL(Args&&... args)
{
    std::ostringstream stream;
#ifndef NDEBUG
    stream << "\033[1;93m[DEBUG MODE]\033[1;0m ";
#endif
    stream << "\033[1;92m[INFO] \033[1;0m";
    (stream << ... << std::forward<Args>(args));

    std::cout << stream.str() << std::endl;
}