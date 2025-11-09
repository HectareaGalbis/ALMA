
#pragma once

#include "objects.hpp"

void intern_macros();

#define declare_macro(name)                                                                         \
    class name : public Macro {                                                                     \
    public:                                                                                         \
        template <typename Name>                                                                    \
        name(Name&& _n)                                                                             \
            : Macro(std::forward<Name>(_n))                                                         \
        {                                                                                           \
        }                                                                                           \
                                                                                                    \
    protected:                                                                                      \
        virtual std::shared_ptr<Object> eval_body(const std::vector<std::shared_ptr<Object>>& args, \
            Environment& lex_env) override;                                                         \
    }

declare_macro(defun);
declare_macro(defmacro);
