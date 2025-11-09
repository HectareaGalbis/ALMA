
#pragma once

#include <filesystem>
#include <sstream>

// TOKENPASTE
#define TOKENPASTE2(X, Y) X##Y
#define TOKENPASTE(X, Y) TOKENPASTE2(X, Y)

// GENSYM
#define gensym(X) TOKENPASTE(X, __LINE__)

// DEBUG
#ifdef NDEBUG
#define debugMsg(M) void(0)
#else
#define debugMsg(M)                                                                                              \
    std::stringstream gensym(__output__);                                                                        \
    gensym(__output__) << std::filesystem::path(__FILE__).filename().native() << " (" << __LINE__ << "): " << M; \
    std::cout << gensym(__output__).str() << std::endl;
#endif
