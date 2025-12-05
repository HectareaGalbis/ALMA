
#include "procedure.hpp"

std::shared_ptr<Object> Procedure::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

void Procedure::emit_impl() const
{
    throw std::runtime_error("A procedure cannot be emitted");
}

std::string Procedure::to_string_impl() const
{
    std::stringstream s;
    s << "<";
    s << std::hex << this;
    if (this->name)
        s << " " << *this->name;
    s << ">";
    return s.str();
}

bool Procedure::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "procedure";
}

std::vector<std::shared_ptr<Object>> Function::eval_args(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env)
{
    std::vector<std::shared_ptr<Object>> evaluated_args;
    evaluated_args.reserve(args.size());
    for (const std::shared_ptr<Object>& arg : args) {
        evaluated_args.push_back(Object::eval(arg, lex_env));
    }

    return evaluated_args;
}

std::shared_ptr<Object> Function::apply(
    Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments)
{
    std::vector<std::shared_ptr<Object>> evaluated_args = eval_args(arguments, lex_env);
    return this->eval_body(evaluated_args, lex_env);
}

bool Function::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "function" || this->Procedure::typep_impl(sym);
}

std::shared_ptr<Object> FunctionUser::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (this->params.size() != args.size())
        throw std::runtime_error("Needed " + std::to_string(this->params.size()) + " but received " + std::to_string(args.size()) + " params");

    this->closure.pushValues(this->params, args);

    for (size_t i = 0; i < this->body.size() - 1; i++) {
        Object::eval(this->body[i], this->closure);
    }
    std::shared_ptr<Object> result = Object::eval(this->body.back(), this->closure);

    this->closure.popValues();

    return result;
}

bool FunctionUser::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "function-user" || this->Function::typep_impl(sym);
}

std::shared_ptr<Object> Macro::apply(
    Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments)
{
    std::shared_ptr<Object> result = this->eval_body(arguments, lex_env);
    return Object::eval(result, lex_env);
}

std::shared_ptr<Object> Macro::expand(Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments)
{
    return this->eval_body(arguments, lex_env);
}

bool Macro::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "macro" || this->Procedure::typep_impl(sym);
}

std::shared_ptr<Object> MacroUser::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (this->params.size() != args.size())
        throw std::runtime_error("Needed " + std::to_string(this->params.size()) + " but received " + std::to_string(args.size()) + " params");

    this->closure.pushValues(this->params, args);

    for (size_t i = 0; i < this->body.size() - 1; i++) {
        Object::eval(this->body[i], this->closure);
    }
    std::shared_ptr<Object> result = Object::eval(this->body.back(), this->closure);

    this->closure.popValues();

    return result;
}

bool MacroUser::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "macro-user" || this->Macro::typep_impl(sym);
}
