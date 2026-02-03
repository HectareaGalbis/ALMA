
#include "procedure.hpp"
#include "alma.hpp"
#include "cons.hpp"
#include "debug.hpp"
#include "environment.hpp"
#include "package.hpp"
#include "symbol.hpp"
#include <stdexcept>

void Procedure::check_types(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    const std::vector<std::string>& alma_types)
{
    if (arg_list.size() != alma_types.size())
        mthrow("Expected " << arg_list.size() << " arguments but received " << alma_types.size());

    for (size_t i = 0; i < arg_list.size(); i++) {
        if (!this->alma.alma_typep(arg_list[i], alma_types[i]))
            mthrow("The argument number " << i << " must be a " << alma_types[i]);
    }
}

void Procedure::check_types(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    const std::vector<std::string>& alma_types,
    const std::string& alma_rest_type)
{
    if (arg_list.size() < alma_types.size())
        mthrow("Expected at least " << arg_list.size() << " arguments but received " << alma_types.size());

    for (size_t i = 0; i < alma_types.size(); i++) {
        if (!this->alma.alma_typep(arg_list[i], alma_types[i]))
            mthrow("The argument number " << i << " must be a " << alma_types[i]);
    }
    if (alma_rest_type != "t") {
        for (size_t i = alma_types.size(); i < arg_list.size(); i++) {
            if (!this->alma.alma_typep(arg_list[i], alma_rest_type))
                mthrow("The argument number " << i << " must be a " << alma_types[i]);
        }
    }
}

Procedure::Procedure(Alma& _alma)
    : Object(_alma)
{
}

bool Procedure::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("procedure") || this->Object::typep(self, type);
}

ObjectWeakRef<Object> Procedure::apply(
    ObjectWeakRef<Object> self [[maybe_unused]],
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> enviroment)
{
    return this->eval_body(arg_list, enviroment);
}

// -----------------------------------------------------------------------------

Function::Function(Alma& _alma)
    : Procedure(_alma)
{
}

ObjectWeakRef<Object> Function::apply(
    ObjectWeakRef<Object> self [[maybe_unused]],
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> enviroment)
{
    std::vector<ObjectWeakRef<Object>> eval_arg_list;
    for (const ObjectWeakRef<Object>& arg : arg_list)
        eval_arg_list.push_back(this->alma.eval(arg));

    return this->eval_body(arg_list, enviroment);
}

bool Function::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("function") || this->Procedure::typep(self, type);
}

// -----------------------------------------------------------------------------

Macro::Macro(Alma& _alma)
    : Procedure(_alma)
{
}

ObjectWeakRef<Object> Macro::expand(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> enviroment)
{
    return this->eval_body(arg_list, enviroment);
}

bool Macro::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("macro") || this->Procedure::typep(self, type);
}

ObjectWeakRef<Object> Macro::apply(
    ObjectWeakRef<Object> self [[maybe_unused]],
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> enviroment)
{
    return this->alma.eval(this->expand(arg_list, enviroment));
}

// -----------------------------------------------------------------------------

FunctionUser::FunctionUser(
    Alma& _alma,
    const std::vector<ObjectWeakRef<Object>>& _param_list,
    const std::optional<ObjectWeakRef<Object>>& _param_rest,
    ObjectWeakRef<Environment> _closure,
    const std::vector<ObjectWeakRef<Object>>& _body)
    : Function(_alma)
    , closure(*this, _closure)
{
    for (ObjectWeakRef<Object> param : _param_list)
        this->param_list.emplace_back(*this, param);
    if (_param_rest)
        this->param_rest.emplace(*this, *_param_rest);
    for (ObjectWeakRef<Object> expr : _body)
        this->body.emplace_back(*this, expr);
}

ObjectWeakRef<Object> FunctionUser::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> enviroment [[maybe_unused]])
{
    if (arg_list.size() < this->param_list.size()) {
        if (this->param_rest) {
            mthrow("Expected at least " << this->param_list.size() << " arguments");
        } else {
            mthrow("Expected " << this->param_list.size() << " arguments");
        }
    } else if (!this->param_rest && arg_list.size() > this->param_list.size()) {
        mthrow("Expected " << this->param_list.size() << " arguments");
    }

    std::vector<ObjectWeakRef<Object>> arg_rest;
    if (this->param_rest) {
        for (size_t i = this->param_list.size(); i < arg_list.size(); i++)
            arg_rest.push_back(arg_list[i]);
    }
    {
        Environment::WithLayer layer(*this->closure);

        for (size_t i = 0; i < this->param_list.size(); i++)
            this->closure->insert_or_set_value(
                this->param_list[i], alma.find_alma_symbol("value"), arg_list[i]);
        if (this->param_rest)
            this->closure->insert_or_set_value(
                *this->param_rest, alma.find_alma_symbol("value"), this->alma.make<Cons>(arg_rest));

        for (size_t i = 0; i < this->body.size() - 1; i++)
            alma.eval(this->body[i], this->closure);
        return alma.eval(this->body.back(), this->closure);
    }
}

bool FunctionUser::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return type == alma.find_alma_symbol("function-user") || this->Function::typep(self, type);
}

// -----------------------------------------------------------------------------

MacroUser::MacroUser(
    Alma& _alma,
    const std::vector<ObjectWeakRef<Object>>& _param_list,
    const std::optional<ObjectWeakRef<Object>>& _param_rest,
    ObjectWeakRef<Environment> _closure,
    const std::vector<ObjectWeakRef<Object>>& _body)
    : Macro(_alma)
    , closure(*this, _closure)
{
    for (ObjectWeakRef<Object> param : _param_list)
        this->param_list.emplace_back(*this, param);
    if (_param_rest)
        this->param_rest.emplace(*this, *_param_rest);
    for (ObjectWeakRef<Object> expr : _body)
        this->body.emplace_back(*this, expr);
}

ObjectWeakRef<Object> MacroUser::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> enviroment [[maybe_unused]])
{
    if (arg_list.size() < this->param_list.size()) {
        if (this->param_rest) {
            mthrow("Expected at least " << this->param_list.size() << " arguments");
        } else {
            mthrow("Expected " << this->param_list.size() << " arguments");
        }
    } else if (!this->param_rest && arg_list.size() > this->param_list.size()) {
        mthrow("Expected " << this->param_list.size() << " arguments");
    }

    std::vector<ObjectWeakRef<Object>> arg_rest;
    if (this->param_rest) {
        for (size_t i = this->param_list.size(); i < arg_list.size(); i++)
            arg_rest.push_back(arg_list[i]);
    }
    {
        Environment::WithLayer layer(*this->closure);

        for (size_t i = 0; i < this->param_list.size(); i++)
            this->closure->insert_or_set_value(
                this->param_list[i], alma.find_alma_symbol("value"), arg_list[i]);
        if (this->param_rest)
            this->closure->insert_or_set_value(
                *this->param_rest, alma.find_alma_symbol("value"), this->alma.make<Cons>(arg_rest));

        for (size_t i = 0; i < this->body.size() - 1; i++)
            alma.eval(this->body[i], this->closure);
        return alma.eval(this->body.back(), this->closure);
    }
}

bool MacroUser::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return alma.eq(type, alma.find_alma_symbol("macro-user")) || this->Procedure::typep(self, type);
}

// -----------------------------------------------------------------------------
