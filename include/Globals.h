#ifndef SEAHORSE_GLOBALS
#define SEAHORSE_GLOBALS

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

#define S_INFO(...) S_LOG_IMPL(__LINE__, __FILE__, "[INFO] ",__VA_ARGS__)
#define S_ERROR(...) S_LOG_IMPL(__LINE__, __FILE__, "[ERROR]",__VA_ARGS__)

template <typename ...Args>
void S_LOG_IMPL(int line, const char* fileName, const char* type, Args&& ...args) {
    std::ostringstream stream;
    stream <<fileName << ":" << line << " " << type << " ";
    (stream << ... << std::forward<Args>(args)) << '\n';

    printf(stream.str().c_str());
}
#endif // SEAHORSE_GLOBALS