
#include "function.hpp"
#include "objects.hpp"
#include "package.hpp"
#include <iostream>

#define intern_function(name, sym_name)                                                   \
    std::shared_ptr<Symbol>& name##_func = Package::almaPackage->intern_symbol(sym_name); \
    name##_func->function = std::make_shared<name>(sym_name);

void intern_functions()
{
    intern_function(sum, "+");
    intern_function(print, "print");
    intern_function(typep, "typep");
    intern_function(set_symbol_function, "set-symbol-function");
    intern_function(set_symbol_package, "set-symbol-package");
    intern_function(emit, "emit");
    intern_function(car, "car");
    intern_function(cdr, "cdr");
    intern_function(setq, "setq");
    intern_function(eq, "eq");
    intern_function(eql, "eql");
    intern_function(macroexpand_1, "macroexpand-1");
    intern_function(eval, "eval");
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> sum::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    int64_t sum_value = 0;
    for (const std::shared_ptr<Object>& arg : args) {
        std::shared_ptr<Integer> value = std::dynamic_pointer_cast<Integer>(arg);
        if (!value)
            throw std::runtime_error("Expected an integer");
        sum_value += value->value;
    }

    return std::make_shared<Integer>(sum_value);
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> print::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 1)
        throw std::runtime_error("Expected only one argument.");

    std::cout << Object::to_string(args.front()) << std::endl;

    return args.front();
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> typep::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 2)
        throw std::runtime_error("Expected two arguments.");

    std::shared_ptr<Symbol> sym = std::dynamic_pointer_cast<Symbol>(args[1]);
    if (!sym)
        throw std::runtime_error("The second argument must be a symbol.");

    if (Object::typep(args[0], sym)) {
        return (*Package::almaPackage->find_symbol("t"))->values.back();
    } else {
        return (*Package::almaPackage->find_symbol("nil"))->values.back();
    }
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> set_symbol_function::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 2)
        throw std::runtime_error("Expected two arguments.");

    std::shared_ptr<Symbol> sym = std::dynamic_pointer_cast<Symbol>(args[0]);
    if (!sym)
        throw std::runtime_error("The first argument must be a symbol.");

    std::shared_ptr<Procedure> proc = std::dynamic_pointer_cast<Procedure>(args[1]);
    if (!proc)
        throw std::runtime_error("The second argument must be a valid procedure.");

    sym->function = proc;

    return proc;
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> set_symbol_package::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 2)
        throw std::runtime_error("Expected two arguments.");

    std::shared_ptr<Symbol> sym = std::dynamic_pointer_cast<Symbol>(args[0]);
    if (!sym)
        throw std::runtime_error("The first argument must be a symbol.");

    std::shared_ptr<Package> package = std::dynamic_pointer_cast<Package>(args[1]);
    if (!package)
        throw std::runtime_error("The second argument must be a valid package.");

    sym->package = package;

    return package;
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> setq::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env)
{
    if (args.size() != 2)
        throw std::runtime_error("Expected two arguments.");

    std::shared_ptr<Symbol> sym = std::dynamic_pointer_cast<Symbol>(args[0]);
    if (!sym)
        throw std::runtime_error("The first argument must be a symbol.");

    if (lex_env.isSymbolBound(sym))
        lex_env.setValue(sym, args[1]);
    else if (!sym->values.empty())
        sym->values.back() = args[1];
    else
        throw std::runtime_error("The symbol " + sym->name + " is not bound.");

    return args[1];
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> emit::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 1)
        throw std::runtime_error("Expected one argument.");

    Object::emit(args[0]);

    return args[0];
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> car::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 1)
        throw std::runtime_error("Expected one argument.");

    std::shared_ptr<Cons> pair = std::dynamic_pointer_cast<Cons>(args[0]);
    if (!pair)
        throw std::runtime_error("Expected a cons.");

    return pair->car;
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> cdr::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 1)
        throw std::runtime_error("Expected one argument.");

    std::shared_ptr<Cons> pair = std::dynamic_pointer_cast<Cons>(args[0]);
    if (!pair)
        throw std::runtime_error("Expected a cons.");

    return pair->cdr;
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> eq::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 2)
        throw std::runtime_error("Expected two arguments.");

    if (Object::eq(args[0], args[1]))
        return *Package::almaPackage->find_symbol("t");
    else
        return std::make_shared<Nil>();
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> eql::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (args.size() != 2)
        throw std::runtime_error("Expected two arguments.");

    std::shared_ptr<Integer> i1 = std::dynamic_pointer_cast<Integer>(args[0]);
    std::shared_ptr<Integer> i2 = std::dynamic_pointer_cast<Integer>(args[1]);

    if (!i1 || !i2)
        throw std::runtime_error("Expected integers as arguments");

    if (i1->value == i2->value)
        return *Package::almaPackage->find_symbol("t");
    else
        return std::make_shared<Nil>();
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> macroexpand_1::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env)
{
    if (args.size() != 1)
        throw std::runtime_error("Expected one argument.");

    std::shared_ptr<Cons> macroList = std::dynamic_pointer_cast<Cons>(args[0]);
    if (!macroList)
        throw std::runtime_error("Expected a list");

    std::shared_ptr<Symbol> macroname = std::dynamic_pointer_cast<Symbol>(macroList->car);
    if (!macroname)
        throw std::runtime_error("Expected a symbol as the first element");

    std::shared_ptr<Macro> macro = std::dynamic_pointer_cast<Macro>(macroname->function);
    if (macro) {
        std::shared_ptr<Cons> macroargs = std::dynamic_pointer_cast<Cons>(macroList->cdr);
        if (!macroargs)
            throw std::runtime_error("Expected a list of arguments to the macro");
        return macro->expand(lex_env, macroargs->toList());
    } else {
        return args[0];
    }
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> eval::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env)
{
    if (args.size() != 1)
        throw std::runtime_error("Expected one argument.");

    return Object::eval(args[0], lex_env);
}
