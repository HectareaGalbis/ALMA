
#pragma once

#include "compiler.hpp"
#include "grammar.hpp"
#include "objects.hpp"
#include "package.hpp"
#include <iostream>
#include <memory>
#include <tao/pegtl.hpp>

namespace peg = tao::pegtl;

template <typename Rule>
struct make_ast;

class ast {
private:
    std::vector<std::vector<std::shared_ptr<Object>>> expressions_stack;

public:
    ast()
    {
    }

    void push_layer()
    {
        this->expressions_stack.emplace_back();
    }

    std::vector<std::shared_ptr<Object>> pop_layer()
    {
        std::vector<std::shared_ptr<Object>> expressions = this->expressions_stack.back();
        this->expressions_stack.pop_back();
        return expressions;
    }

    template <std::derived_from<Object> T>
    void add_expr(const std::shared_ptr<T>& expression)
    {
        this->expressions_stack.back().push_back(std::static_pointer_cast<Object>(expression));
    }

    // void read()
    // {
    //     expressions_stack.clear();
    //     this->push_layer();
    //     char* line = linenoise("tlang> ");
    //     peg::string_input in(line, "stdin");
    //     peg::parse<lex::tlang, make_ast>(in, *this);
    //     linenoiseHistoryAdd(line);
    //     linenoiseFree(line);
    // }

    void read(const std::filesystem::path& file)
    {
        expressions_stack.clear();
        this->push_layer();
        peg::file_input in(file);
        peg::parse<lex::tlang, make_ast>(in, *this);
    }

    void eval(Compiler& comp, lexical_environment& lex_env)
    {
        for (std::shared_ptr<Object>& expression : this->expressions_stack.front()) {
            expression = Object::eval(expression, comp, lex_env);
        }
    }

    void print() const
    {
        if (this->expressions_stack.front().empty()) {
            std::cout << ";; No values" << std::endl;
        } else {
            for (const std::shared_ptr<Object>& expression : this->expressions_stack.front()) {
                Object::print(expression);
                std::cout << std::endl;
            }
        }
    }
};

template <typename Rule>
struct make_ast {
    template <typename ActionInput>
    static void apply(ActionInput& in [[maybe_unused]], ast& ast [[maybe_unused]]) { }
};

// integer
template <>
struct make_ast<lex::integer> {
    template <typename ActionInput>
    static void apply(ActionInput& in, ast& ast)
    {
        int64_t value = atoll(in.string().c_str());
        ast.add_expr(std::make_shared<Integer>(value));
    }
};

// symbol
template <>
struct make_ast<lex::symbol> {
    template <typename ActionInput>
    static void apply(ActionInput& in, ast& ast)
    {
        std::shared_ptr<Symbol>& sym = package.intern_symbol(in.string());
        ast.add_expr(sym);
    }
};

// list
template <>
struct make_ast<lex::lparen> {
    template <typename ActionInput>
    static void apply(ActionInput& in [[maybe_unused]], ast& ast)
    {
        ast.push_layer();
    }
};

template <>
struct make_ast<lex::rparen> {
    template <typename ActionInput>
    static void apply(ActionInput& in [[maybe_unused]], ast& ast)
    {
        std::vector<std::shared_ptr<Object>> elements = ast.pop_layer();
        if (elements.empty())
            ast.add_expr(std::make_shared<Nil>());
        else
            ast.add_expr(std::make_shared<List>(std::move(elements)));
    }
};
