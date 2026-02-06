
#pragma once

#include "util.hpp"
#include <filesystem>
#include <iostream>
#include <sstream>

#define mthrow(MSG)                                                               \
    {                                                                             \
        std::stringstream gensym(ss);                                             \
        gensym(ss) << std::filesystem::path(__FILE__).filename().native()         \
                   << "(" << __func__ << ":" << __LINE__ << "): " << MSG << "\n"; \
        throw std::runtime_error(gensym(ss).str());                               \
    }

#define massert(COND, MSG) \
    if (!(COND))           \
    mthrow(MSG)

#ifdef NDEBUG
#define debugf(MSG) (void)0
#else
#define debugf(MSG)                                 \
    {                                               \
        std::stringstream gensym(ss);               \
        gensym(ss) << MSG;                          \
        std::cerr << gensym(ss).str() << std::endl; \
    }
#endif

#define athrow(MSG)                                 \
    {                                               \
        std::stringstream gensym(ss);               \
        gensym(ss) << MSG << "\n";                  \
        throw std::runtime_error(gensym(ss).str()); \
    }

#define aassert(COND, MSG) \
    if (!(COND))           \
    athrow(MSG)
