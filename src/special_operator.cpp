
#include "special_operator.hpp"
#include "objects.hpp"
#include "package.hpp"
#include <memory>

#define intern_special_operator(name)                                                \
    std::shared_ptr<Symbol>& name##_so = Package::almaPackage->intern_symbol(#name); \
    name##_so->function = std::make_shared<name>();

void intern_special_operators()
{
    intern_special_operator(progn);
    intern_special_operator(let);
    intern_special_operator(quote);
    intern_special_operator(lambda);
    intern_special_operator(gamma);
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> progn::apply(
    lexical_environment& lex_env [[maybe_unused]],
    const std::vector<std::shared_ptr<Object>>& arguments)
{
    if (arguments.empty()) {
        return std::make_shared<Nil>();
    }
    for (size_t i = 0; i < arguments.size() - 1; i++) {
        Object::eval(arguments[i], lex_env);
    }
    return Object::eval(arguments.back(), lex_env);
}

// --------------------------------------------------------------------------------

static std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>> parseBindings(
    const std::shared_ptr<Cons>& bindings)
{
    std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>> parsedBindings;

    for (const std::shared_ptr<Object>& element : bindings->toList()) {
        const std::shared_ptr<Cons> binding = std::dynamic_pointer_cast<Cons>(element);
        if (!binding)
            throw std::runtime_error("Expected a binding clause (a list).");
        std::vector<std::shared_ptr<Object>> bindingList = binding->toList();
        if (bindingList.size() != 2)
            throw std::runtime_error("The binding clause must have 2 elements.");
        const std::shared_ptr<Symbol> var = std::dynamic_pointer_cast<Symbol>(bindingList[0]);
        if (!var)
            throw std::runtime_error("The first element of the binding clause must be a symbol");
        const std::shared_ptr<Object>& value = bindingList[1];
        parsedBindings.emplace_back(std::move(var), value);
    }

    return parsedBindings;
}

static std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>> evaluateBindings(
    lexical_environment& lex_env,
    const std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>>& bindings)
{
    std::vector<std::pair<std::shared_ptr<Symbol>, std::shared_ptr<Object>>> evaluatedBindings;

    for (const auto& [var, value] : bindings) {
        evaluatedBindings.emplace_back(var, Object::eval(value, lex_env));
    }

    return evaluatedBindings;
}

std::shared_ptr<Object> let::apply(
    lexical_environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments)
{
    if (arguments.empty())
        throw std::runtime_error("let needs at least a list");

    const std::shared_ptr<Cons> bindings = std::dynamic_pointer_cast<Cons>(arguments.front());
    if (!bindings) {
        Object::print(arguments.front());
        throw std::runtime_error("Expected a list.");
    }
    auto parsedBindings = parseBindings(bindings);
    auto evaluatedBindings = evaluateBindings(lex_env, parsedBindings);

    if (arguments.size() == 1)
        return std::make_shared<Nil>();

    for (const auto& [var, value] : evaluatedBindings)
        lex_env.push_value(var, value);
    for (size_t i = 1; i < arguments.size() - 1; i++)
        Object::eval(arguments[i], lex_env);
    std::shared_ptr<Object> result = Object::eval(arguments.back(), lex_env);
    for (const auto& [var, value] : evaluatedBindings)
        lex_env.pop_value(var);

    return result;
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> quote::apply(
    lexical_environment& lex_env [[maybe_unused]],
    const std::vector<std::shared_ptr<Object>>& arguments)
{
    if (arguments.size() != 1)
        throw std::runtime_error("Expected only one argument.");
    return arguments[0];
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> lambda::apply(
    lexical_environment& lex_env [[maybe_unused]],
    const std::vector<std::shared_ptr<Object>>& arguments)
{
    if (arguments.size() < 1)
        throw std::runtime_error("Expected at least one argument.");

    std::shared_ptr<Cons> func_args = std::dynamic_pointer_cast<Cons>(arguments[0]);
    if (!func_args)
        throw std::runtime_error("Expected a list of symbols.");

    std::vector<std::shared_ptr<Symbol>> func_arg_symbols;
    for (std::shared_ptr<Object>& func_arg : func_args->toList()) {
        std::shared_ptr<Symbol> func_arg_symbol = std::dynamic_pointer_cast<Symbol>(func_arg);
        if (!func_arg_symbol)
            throw std::runtime_error("Expected a symbol as an argument.");
        func_arg_symbols.push_back(func_arg_symbol);
    }

    std::vector<std::shared_ptr<Object>> body;
    for (size_t i = 1; i < arguments.size(); i++)
        body.push_back(arguments[i]);

    return std::make_shared<FunctionUser>("<lambda>", func_arg_symbols, body);
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> gamma::apply(
    lexical_environment& lex_env [[maybe_unused]],
    const std::vector<std::shared_ptr<Object>>& arguments)
{
    if (arguments.size() < 1)
        throw std::runtime_error("Expected at least one argument.");

    std::shared_ptr<Cons> macro_args = std::dynamic_pointer_cast<Cons>(arguments[0]);
    if (!macro_args)
        throw std::runtime_error("Expected a list of symbols.");

    std::vector<std::shared_ptr<Symbol>> macro_arg_symbols;
    for (std::shared_ptr<Object>& macro_arg : macro_args->toList()) {
        std::shared_ptr<Symbol> macro_arg_symbol = std::dynamic_pointer_cast<Symbol>(macro_arg);
        if (!macro_arg_symbol)
            throw std::runtime_error("Expected a symbol as an argument.");
        macro_arg_symbols.push_back(macro_arg_symbol);
    }

    std::vector<std::shared_ptr<Object>> body;
    for (size_t i = 1; i < arguments.size(); i++)
        body.push_back(arguments[i]);

    return std::make_shared<MacroUser>("<lambda>", macro_arg_symbols, body);
}
