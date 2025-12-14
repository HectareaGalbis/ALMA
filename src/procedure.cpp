
#include "procedure.hpp"
#include "alma.hpp"
#include "cons.hpp"
#include "environment.hpp"
#include "package.hpp"
#include <stdexcept>

bool Procedure::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const
{
    return type == alma.find_alma_symbol("procedure") || this->Object::typep(self, type, alma);
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Function::apply(ObjectWeakRef<Cons> arguments, Alma& alma)
{
    std::vector<ObjectWeakRef<Object>> evaluated_args;
    alma_for_each(arg, arguments, alma)
    {
        evaluated_args.emplace_back(alma.eval(arg));
    }
    return this->eval_body(evaluated_args, alma);
}

bool Function::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const
{
    return type == alma.alma_package->find_symbol("function", alma) || this->Procedure::typep(self, type, alma);
}

// --------------------------------------------------------------------------------

FunctionUser::FunctionUser(ObjectWeakRef<Environment> _closure,
    const std::vector<ObjectWeakRef<Symbol>>& _params,
    const std::vector<ObjectWeakRef<Object>>& _body)
    : closure(*this, _closure)
{
    for (ObjectWeakRef<Symbol> param : _params)
        this->params.emplace_back(*this, param);
    for (ObjectWeakRef<Object> expr : _body)
        this->body.emplace_back(*this, expr);
}

ObjectWeakRef<Object> FunctionUser::eval_body(const std::vector<ObjectWeakRef<Object>>& args, Alma& alma)
{
    if (this->params.size() != args.size())
        throw std::runtime_error("Needed " + std::to_string(this->params.size()) + " but received " + std::to_string(args.size()) + " params");

    this->closure->pushValues(this->params, args, alma);

    for (size_t i = 0; i < this->body.size() - 1; i++) {
        Object::eval(this->body[i], this->closure);
    }
    std::shared_ptr<Object> result = Object::eval(this->body.back(), this->closure);

    this->closure.popValues();

    return result;
}

bool FunctionUser::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const
{
    return type == alma.alma_package->find_symbol("function-user", alma)
        || this->Function::typep(self, type, alma);
}

// --------------------------------------------------------------------------------

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
