
#pragma once

#include "compiler.hpp"
#include "environment.hpp"
#include <memory>
#include <optional>
#include <vector>

class Object {
public:
    static std::shared_ptr<Object> eval(
        const std::shared_ptr<Object>& obj, Compiler& comp, lexical_environment& lex_env);
    static void print(const std::shared_ptr<Object>& obj);
    static bool is_true(std::shared_ptr<Object>& obj);
    static bool eq(std::shared_ptr<Object>& obj1, std::shared_ptr<Object>& obj2);

protected:
    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, Compiler& comp, lexical_environment& lex_env) const
        = 0;
    virtual void print_impl() const = 0;
    virtual operator bool() const { return true; }
};

// Integer
struct Integer : Object {
    int64_t value;

    Integer(int64_t _value);

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, Compiler& comp, lexical_environment& lex_env) const override;
    virtual void print_impl() const override;
};

// procedure
struct Procedure : Object {
    std::optional<std::string> name;

    Procedure() = default;
    Procedure(const Procedure& other) = default;
    template <typename Name>
    Procedure(Name&& _name)
        : name(_name)
    {
    }

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, Compiler& comp, lexical_environment& lex_env) const override;
    virtual void print_impl() const override;

    virtual std::shared_ptr<Object> apply(
        Compiler& comp,
        lexical_environment& lex_env,
        const std::vector<std::shared_ptr<Object>>& arguments)
        = 0;
};

struct Function : Procedure {
    template <typename Name>
    Function(Name&& _name)
        : Procedure(std::forward<Name>(_name))
    {
    }

private:
    static std::vector<std::shared_ptr<Object>> eval_args(
        const std::vector<std::shared_ptr<Object>>& args,
        Compiler& comp,
        lexical_environment& lex_env);

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args,
        Compiler& comp,
        lexical_environment& lex_env) const
        = 0;

public:
    virtual std::shared_ptr<Object> apply(
        Compiler& comp,
        lexical_environment& lex_env,
        const std::vector<std::shared_ptr<Object>>& arguments) override;
};

struct FunctionUser : Function {
    std::vector<std::shared_ptr<Symbol>> params;
    std::vector<std::shared_ptr<Object>> body;

    FunctionUser(const FunctionUser& other) = default;
    template <typename Name, typename Params, typename Body>
    FunctionUser(Name&& _name, Params&& _params, Body&& _body)
        : Function(std::forward<Name>(_name))
        , params(std::forward<Params>(_params))
        , body(std::forward<Body>(_body))
    {
    }

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args,
        Compiler& comp,
        lexical_environment& lex_env) const override;
};

// symbol
struct Symbol : Object {
    std::string name;
    std::vector<std::shared_ptr<Object>> values;
    std::shared_ptr<Procedure> function;

    Symbol(const std::string& _name);

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, Compiler& comp, lexical_environment& lex_env) const override;
    virtual void print_impl() const override;
};

// list
struct List : Object {
    std::vector<std::shared_ptr<Object>> elements;

    template <typename Elements>
    List(Elements&& _elements)
        : elements(std::forward<Elements>(_elements))
    {
    }

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, Compiler& comp, lexical_environment& lex_env) const override;
    virtual void print_impl() const override;
};

// nil
struct Nil : List {
    Nil()
        : List(std::vector<std::shared_ptr<Object>>())
    {
    }

    virtual operator bool() const override { return false; }
};
