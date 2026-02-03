
#pragma once

#include "util.hpp"
#include <filesystem>
#include <sstream>

#define mthrow(MSG)                                                                                          \
    {                                                                                                        \
        std::stringstream gensym(ss);                                                                        \
        gensym(ss) << std::filesystem::path(__FILE__).filename() << " (" << __LINE__ << "):" << MSG << "\n"; \
        throw std::runtime_error(gensym(ss).str());                                                          \
    }

#define massert(COND, MSG) \
    if (!(COND))           \
    mthrow(MSG)
