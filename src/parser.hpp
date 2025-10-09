
#pragma once

#include "grammar.hpp"
#include "objects.hpp"
#include "package.hpp"
#include "reader.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <tao/pegtl.hpp>

namespace peg = tao::pegtl;

template <typename Rule>
struct make_ast;

class ast {
private:
    std::vector<std::shared_ptr<Object>> expressions;

public:
    ast()
    {
    }

    void read(const std::filesystem::path& file)
    {
        std::ifstream file_input(file);
        while (std::shared_ptr<Object> expr = reader::read(file_input))
            expressions.push_back(expr);
    }

    void eval(lexical_environment& lex_env)
    {
        for (std::shared_ptr<Object>& expression : this->expressions) {
            Object::eval(expression, lex_env);
        }
    }

    void print() const
    {
        if (this->expressions.empty()) {
            std::cout << ";; No values" << std::endl;
        } else {
            for (const std::shared_ptr<Object>& expression : this->expressions) {
                Object::print(expression);
                std::cout << std::endl;
            }
        }
    }
};
