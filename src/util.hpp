
#pragma once

// TOKENPASTE
#define TOKENPASTE2(X, Y) X##Y
#define TOKENPASTE(X, Y) TOKENPASTE2(X, Y)

// GENSYM
#define GENSYM(X) TOKENPASTE(X, __LINE__)
