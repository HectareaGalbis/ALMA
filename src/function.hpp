
#pragma once

#include "procedure.hpp"

void intern_functions();

#define declare_function(name)                                                    \
    class name : public Function {                                                \
    public:                                                                       \
        template <typename Name>                                                  \
        name(Name&& _n)                                                           \
            : Function(std::forward<Name>(_n))                                    \
        {                                                                         \
        }                                                                         \
                                                                                  \
    protected:                                                                    \
        virtual ObjectWeakRef<Object> eval_body(                                  \
            const std::vector<ObjectWeakRef<Object>>& args, Alma& alma) override; \
    }

declare_function(sum);
declare_function(print);
declare_function(typep);
declare_function(set_symbol_function);
declare_function(set_symbol_package);
declare_function(setq);
declare_function(emit);
declare_function(car);
declare_function(cdr);
declare_function(eq);
declare_function(eql);
declare_function(macroexpand_1);
declare_function(eval);
