
#pragma once

#include <filesystem>
#include <sstream>

#define tokenpaste_aux(X, Y) X##Y
#define tokenpaste(X, Y) tokenpaste_aux(X, Y)

#define gensym(SYMBOL) tokenpaste(SYMBOL, __LINE__)

#define mthrow(MSG)                                                                                          \
    {                                                                                                        \
        std::stringstream gensym(ss);                                                                        \
        gensym(ss) << std::filesystem::path(__FILE__).filename() << " (" << __LINE__ << "):" << MSG << "\n"; \
        throw std::runtime_error(gensym(ss).str());                                                          \
    }

#define massert(COND, MSG) \
    if (!(COND))           \
    mthrow(MSG)
