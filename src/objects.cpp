
#include "objects.hpp"
#include "package.hpp"
#include <iostream>
#include <optional>

// --------------------------------------------------------------------------------

std::shared_ptr<Object> Object::eval(
    const std::shared_ptr<Object>& obj, Compiler& comp, lexical_environment& lex_env)
{
    return obj->eval_impl(obj, comp, lex_env);
}

void Object::print(const std::shared_ptr<Object>& obj)
{
    obj->print_impl();
}

bool Object::is_true(std::shared_ptr<Object>& obj)
{
    return static_cast<bool>(*obj);
}

bool Object::eq(std::shared_ptr<Object>& obj1, std::shared_ptr<Object>& obj2)
{
    return obj1 == obj2;
}

// --------------------------------------------------------------------------------

Integer::Integer(int64_t _value)
    : value(_value)
{
}

std::shared_ptr<Object> Integer::eval_impl(
    const std::shared_ptr<Object>& obj, Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

void Integer::print_impl() const
{
    std::cout << this->value;
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> Procedure::eval_impl(
    const std::shared_ptr<Object>& obj, Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

void Procedure::print_impl() const
{
    std::cout << "<";
    std::cout << std::hex << this;
    if (this->name)
        std::cout << " " << *this->name;
    std::cout << ">";
}

std::vector<std::shared_ptr<Object>> Function::eval_args(const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp,
    lexical_environment& lex_env)
{
    std::vector<std::shared_ptr<Object>> evaluated_args;
    evaluated_args.reserve(args.size());
    for (const std::shared_ptr<Object>& arg : args) {
        evaluated_args.push_back(Object::eval(arg, comp, lex_env));
    }

    return evaluated_args;
}

std::shared_ptr<Object> Function::apply(
    Compiler& comp,
    lexical_environment& lex_env,
    const std::vector<std::shared_ptr<Object>>& arguments)
{
    std::vector<std::shared_ptr<Object>> evaluated_args = eval_args(arguments, comp, lex_env);
    return this->eval_body(evaluated_args, comp, lex_env);
}

std::shared_ptr<Object> FunctionUser::eval_body(
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp,
    lexical_environment& lex_env) const
{
    if (this->params.size() != args.size())
        throw std::runtime_error("Needed " + std::to_string(this->params.size()) + " but received " + std::to_string(args.size()) + " params");
    for (size_t i = 0; i < args.size(); i++) {
        lex_env.push_value(this->params[i], args[i]);
    }
    for (size_t i = 0; i < this->body.size() - 1; i++) {
        Object::eval(this->body[i], comp, lex_env);
    }
    std::shared_ptr<Object> result = Object::eval(this->body.back(), comp, lex_env);

    for (size_t i = 0; i < args.size(); i++) {
        lex_env.pop_value(this->params[i]);
    }

    return result;
}

// --------------------------------------------------------------------------------

Symbol::Symbol(const std::string& _name)
    : name(_name)
{
}

std::shared_ptr<Object> Symbol::eval_impl(
    const std::shared_ptr<Object>& obj [[maybe_unused]], Compiler& comp [[maybe_unused]], lexical_environment& lex_env [[maybe_unused]]) const
{
    std::optional<std::shared_ptr<Symbol>> maybe_symbol = package.find_symbol(this->name);
    if (maybe_symbol)
        return lex_env.get_value(*maybe_symbol);
    else
        throw std::runtime_error("Symbol not bound.");
}

void Symbol::print_impl() const
{
    std::cout << this->name;
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> List::eval_impl(
    const std::shared_ptr<Object>& obj [[maybe_unused]], Compiler& comp, lexical_environment& lex_env) const
{
    std::shared_ptr<Symbol> func_name = std::dynamic_pointer_cast<Symbol>(elements[0]);
    if (!func_name)
        throw std::runtime_error("Expected a symbol denoting a procedure.");
    if (!func_name->function)
        throw std::runtime_error("The symbol " + func_name->name + " does not denote a procedure.");
    std::vector<std::shared_ptr<Object>> arguments;
    for (size_t i = 1; i < elements.size(); i++)
        arguments.push_back(elements[i]);
    return func_name->function->apply(comp, lex_env, arguments);
}

void List::print_impl() const
{
    std::cout << "(";
    if (!this->elements.empty()) {
        Object::print(this->elements.front());
        for (size_t i = 1; i < this->elements.size(); i++) {
            std::cout << " ";
            Object::print(this->elements[i]);
        }
    }
    std::cout << ")";
}
