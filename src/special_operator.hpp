
#pragma once

#include "objects.hpp"

void intern_special_operators();

#define declare_special_operator(name)                                       \
    class name : public Procedure {                                          \
    public:                                                                  \
        virtual std::shared_ptr<Object> apply(                               \
            Environment& lex_env,                                            \
            const std::vector<std::shared_ptr<Object>>& arguments) override; \
    }

declare_special_operator(progn);
declare_special_operator(let);
declare_special_operator(quote);
declare_special_operator(quasiquote);
declare_special_operator(lambda);
declare_special_operator(gamma);
declare_special_operator(branch);
