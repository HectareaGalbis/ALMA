
#pragma once

#include <iostream>
#include <memory>
#include <ostream>

namespace Emitter {
extern std::unique_ptr<std::ostream> emitter;
template <typename T>
void emit(const T& content);
};

template <typename T>
void Emitter::emit(const T& content)
{
    if (emitter)
        *emitter << content;
    else
        std::cout << content;
}
