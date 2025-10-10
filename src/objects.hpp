
#pragma once

#include "environment.hpp"
#include <memory>
#include <optional>
#include <vector>

class Object {
public:
    static std::shared_ptr<Object> eval(const std::shared_ptr<Object>& obj, lexical_environment& lex_env);
    static void emit(const std::shared_ptr<Object>& obj);
    static void print(const std::shared_ptr<Object>& obj);
    static bool is_true(const std::shared_ptr<Object>& obj);
    static bool eq(const std::shared_ptr<Object>& obj1, const std::shared_ptr<Object>& obj2);
    static bool typep(const std::shared_ptr<Object>& obj, const std::shared_ptr<Symbol>& sym);

protected:
    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, lexical_environment& lex_env) const
        = 0;
    virtual void emit_impl() const = 0;
    virtual void print_impl() const = 0;
    virtual operator bool() const { return true; }
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const = 0;
};

// Integer
struct Integer : Object {
    int64_t value;

    Integer(int64_t _value);

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, lexical_environment& lex_env) const override;
    virtual void emit_impl() const override;
    virtual void print_impl() const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
};

// string
struct String : Object {
    std::string content;

    String(const std::string& content);

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, lexical_environment& lex_env) const override;
    virtual void emit_impl() const override;
    virtual void print_impl() const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
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
        const std::shared_ptr<Object>& obj, lexical_environment& lex_env) const override;
    virtual void emit_impl() const override;
    virtual void print_impl() const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;

    virtual std::shared_ptr<Object> apply(lexical_environment& lex_env,
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
        lexical_environment& lex_env);

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args, lexical_environment& lex_env) const
        = 0;

public:
    virtual std::shared_ptr<Object> apply(
        lexical_environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments) override;

    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
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
        const std::vector<std::shared_ptr<Object>>& args, lexical_environment& lex_env) const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
};

struct Macro : Procedure {
    template <typename Name>
    Macro(Name&& _name)
        : Procedure(std::forward<Name>(_name))
    {
    }

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args, lexical_environment& lex_env) const
        = 0;

public:
    virtual std::shared_ptr<Object> apply(
        lexical_environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments) override;

    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
};

struct MacroUser : Macro {
    std::vector<std::shared_ptr<Symbol>> params;
    std::vector<std::shared_ptr<Object>> body;

    MacroUser(const MacroUser& other) = default;
    template <typename Name, typename Params, typename Body>
    MacroUser(Name&& _name, Params&& _params, Body&& _body)
        : Macro(std::forward<Name>(_name))
        , params(std::forward<Params>(_params))
        , body(std::forward<Body>(_body))
    {
    }

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args, lexical_environment& lex_env) const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
};

// symbol
struct Symbol : Object {
    std::string name;
    std::vector<std::shared_ptr<Object>> values;
    std::shared_ptr<Procedure> function;
    std::shared_ptr<class Package> package;

    Symbol(const std::string& _name);

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, lexical_environment& lex_env) const override;
    virtual void emit_impl() const override;
    virtual void print_impl() const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
};

// cons
struct Cons : Object {
    std::shared_ptr<Object> car;
    std::shared_ptr<Object> cdr;

    Cons(const std::shared_ptr<Object>& _car, const std::shared_ptr<Object>& _cdr);
    Cons(const std::vector<std::shared_ptr<Object>>& list);

    std::vector<std::shared_ptr<Object>> toList() const;

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, lexical_environment& lex_env) const override;
    virtual void emit_impl() const override;
    virtual void print_impl() const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
};

// nil
struct Nil : Object {
    Nil()
    {
    }

    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, lexical_environment& lex_env) const override;
    virtual void emit_impl() const override { }
    virtual operator bool() const override { return false; }
    virtual void print_impl() const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
};
