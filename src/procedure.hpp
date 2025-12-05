
#pragma once

#include "object.hpp"
#include <string>

class Procedure : public Object {
private:
    std::optional<std::string> name;

public:
    Procedure() = default;
    Procedure(const Procedure& other) = default;
    template <typename Name>
    Procedure(Name&& _name)
        : name(_name)
    {
    }

    virtual ObjectRef eval(
        const std::shared_ptr<Object>& obj, Environment& lex_env) const override;
    virtual void emit_impl() const override;
    virtual std::string to_string() const override;
    virtual bool typep(const std::shared_ptr<Symbol>& sym) const override;

    virtual std::shared_ptr<Object> apply(Environment& lex_env,
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
        Environment& lex_env);

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env)
        = 0;

public:
    virtual std::shared_ptr<Object> apply(
        Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments) override;

    virtual bool typep(const std::shared_ptr<Symbol>& sym) const override;
};

struct FunctionUser : Function {
    Environment closure;
    std::vector<std::shared_ptr<Symbol>> params;
    std::vector<std::shared_ptr<Object>> body;

    FunctionUser(const FunctionUser& other) = default;
    template <typename Name, typename Closure, typename Params, typename Body>
    FunctionUser(Name&& _name, Closure&& _closure, Params&& _params, Body&& _body)
        : Function(std::forward<Name>(_name))
        , closure(_closure)
        , params(std::forward<Params>(_params))
        , body(std::forward<Body>(_body))
    {
    }

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env) override;
    virtual bool typep(const std::shared_ptr<Symbol>& sym) const override;
};

struct Macro : Procedure {
    template <typename Name>
    Macro(Name&& _name)
        : Procedure(std::forward<Name>(_name))
    {
    }

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env)
        = 0;

public:
    virtual std::shared_ptr<Object> apply(
        Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments) override;
    std::shared_ptr<Object> expand(Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments);

    virtual bool typep(const std::shared_ptr<Symbol>& sym) const override;
};

struct MacroUser : Macro {
    Environment closure;
    std::vector<std::shared_ptr<Symbol>> params;
    std::vector<std::shared_ptr<Object>> body;

    MacroUser(const MacroUser& other) = default;
    template <typename Name, typename Closure, typename Params, typename Body>
    MacroUser(Name&& _name, Closure&& _closure, Params&& _params, Body&& _body)
        : Macro(std::forward<Name>(_name))
        , closure(_closure)
        , params(std::forward<Params>(_params))
        , body(std::forward<Body>(_body))
    {
    }

protected:
    virtual std::shared_ptr<Object> eval_body(
        const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env) override;
    virtual bool typep(const std::shared_ptr<Symbol>& sym) const override;
};
