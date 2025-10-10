
#pragma once

#include "objects.hpp"

void intern_functions();

#define declare_function(name)                                                                      \
    class name : public Function {                                                                  \
    public:                                                                                         \
        template <typename Name>                                                                    \
        name(Name&& _n)                                                                             \
            : Function(std::forward<Name>(_n))                                                      \
        {                                                                                           \
        }                                                                                           \
                                                                                                    \
    protected:                                                                                      \
        virtual std::shared_ptr<Object> eval_body(const std::vector<std::shared_ptr<Object>>& args, \
            lexical_environment& lex_env) const override;                                           \
    }

declare_function(sum);
declare_function(print);
declare_function(typep);
declare_function(set_symbol_function);
declare_function(set_symbol_package);
declare_function(emit);
