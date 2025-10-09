
#include "function.hpp"
#include "package.hpp"
#include "registers.hpp"
#include <fstream>
#include <iostream>

#define intern_function(name, sym_name)                                      \
    std::shared_ptr<Symbol>& name##_func = package.intern_symbol(#sym_name); \
    name##_func->function = std::make_shared<name>(#sym_name);

void intern_functions()
{
    intern_function(sum, sum);
    intern_function(print, print);
    intern_function(typep, typep);
    intern_function(set_symbol_function, set_symbol_function);
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> sum::eval_body(
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]],
    std::ofstream& output_file [[maybe_unused]]) const
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
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]],
    std::ofstream& output_file [[maybe_unused]]) const
{
    if (args.size() != 1)
        throw std::runtime_error("Expected only one argument.");

    std::cout << std::endl;
    Object::print(args.front());

    return args.front();
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> typep::eval_body(
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]],
    std::ofstream& output_file [[maybe_unused]]) const
{
    if (args.size() != 2)
        throw std::runtime_error("Expected two arguments.");

    std::shared_ptr<Symbol> sym = std::dynamic_pointer_cast<Symbol>(args[1]);
    if (!sym)
        throw std::runtime_error("The second argument must be a symbol.");

    if (Object::typep(args[0], sym)) {
        return (*package.find_symbol("t"))->values.back();
    } else {
        return (*package.find_symbol("nil"))->values.back();
    }
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> set_symbol_function::eval_body(
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]],
    std::ofstream& output_file [[maybe_unused]]) const
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

std::shared_ptr<Object> emit::eval_body(
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]],
    std::ofstream& output_file [[maybe_unused]]) const
{
    if (args.size() != 1)
        throw std::runtime_error("Expected one argument.");

    Object::emit(args[0]);

    return args[0];
}
