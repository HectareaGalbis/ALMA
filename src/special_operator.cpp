
#include "special_operator.hpp"
#include "objects.hpp"
#include "package.hpp"
#include <memory>
#include <sstream>

#define intern_special_operator(name)                                  \
    std::shared_ptr<Symbol>& name##_so = package.intern_symbol(#name); \
    name##_so->function = std::make_shared<name>();

void intern_special_operators()
{
    intern_special_operator(progn);
    intern_special_operator(let);
    intern_special_operator(quote);
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> progn::apply(
    Compiler& comp,
    lexical_environment& lex_env [[maybe_unused]],
    const std::vector<std::shared_ptr<Object>>& arguments)
{
    if (arguments.empty()) {
        return std::make_shared<Nil>();
    }
    for (size_t i = 0; i < arguments.size() - 1; i++) {
        Object::eval(arguments[i], comp, lex_env);
    }
    return Object::eval(arguments.back(), comp, lex_env);
}

// --------------------------------------------------------------------------------

static std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>> parseBindings(
    const std::shared_ptr<List>& bindings)
{
    std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>> parsedBindings;

    for (const std::shared_ptr<Object>& element : bindings->elements) {
        const std::shared_ptr<List> binding = std::dynamic_pointer_cast<List>(element);
        if (!binding)
            throw std::runtime_error("Expected a binding clause (a list).");
        if (binding->elements.size() != 2)
            throw std::runtime_error("The binding clause must have 2 elements.");
        const std::shared_ptr<Symbol> var = std::dynamic_pointer_cast<Symbol>(binding->elements[0]);
        if (!var)
            throw std::runtime_error("The first element of the binding clause must be a symbol");
        const std::shared_ptr<Object>& value = binding->elements[1];
        parsedBindings.emplace_back(std::move(var), value);
    }

    return parsedBindings;
}

static std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>> evaluateBindings(
    Compiler& comp,
    lexical_environment& lex_env,
    const std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>>& bindings)
{
    std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>> evaluatedBindings;

    for (const auto& [var, value] : bindings) {
        evaluatedBindings.emplace_back(var, Object::eval(value, comp, lex_env));
    }

    return evaluatedBindings;
}

std::shared_ptr<Object> let::apply(
    Compiler& comp,
    lexical_environment& lex_env,
    const std::vector<std::shared_ptr<Object>>& arguments)
{
    if (arguments.empty())
        throw std::runtime_error("let needs at least a list");

    const std::shared_ptr<List> bindings = std::dynamic_pointer_cast<List>(arguments.front());
    if (!bindings)
        throw std::runtime_error("Expected a list.");
    auto parsedBindings = parseBindings(bindings);
    auto evaluatedBindings = evaluateBindings(comp, lex_env, parsedBindings);

    if (arguments.size() == 1)
        return std::make_shared<Nil>();

    for (const auto& [var, value] : evaluatedBindings)
        lex_env.push_value(var, value);
    for (size_t i = 1; i < arguments.size() - 1; i++)
        Object::eval(arguments[i], comp, lex_env);
    std::shared_ptr<Object> result = Object::eval(arguments.back(), comp, lex_env);
    for (const auto& [var, value] : evaluatedBindings)
        lex_env.pop_value(var);

    return result;
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> quote::apply(
    Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]],
    const std::vector<std::shared_ptr<Object>>& arguments)
{
    if (arguments.size() != 1)
        throw std::runtime_error("Expected only one argument.");
    return arguments[0];
}
