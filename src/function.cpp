
#include "function.hpp"
#include "package.hpp"
#include "registers.hpp"
#include <iostream>

#define intern_function(name, sym_name)                                      \
    std::shared_ptr<Symbol>& name##_func = package.intern_symbol(#sym_name); \
    name##_func->function = std::make_shared<name>(#sym_name);

void intern_functions()
{
    intern_function(sum, sum);
    intern_function(print, print);
    intern_function(mov, mov);
    intern_function(Syscall, syscall);
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> sum::eval_body(
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp [[maybe_unused]],
    lexical_environment& lex_env [[maybe_unused]]) const
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
    lexical_environment& lex_env [[maybe_unused]]) const
{
    if (args.size() != 1)
        throw std::runtime_error("Expected only one argument.");

    std::cout << std::endl;
    Object::print(args.front());

    return args.front();
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> mov::eval_body(
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp,
    lexical_environment& lex_env [[maybe_unused]]) const
{
    if (args.size() != 2)
        throw std::runtime_error("Expected two arguments.");

    std::shared_ptr<Symbol> sym = std::dynamic_pointer_cast<Symbol>(args[0]);

    if (!Registers::reg64.contains(sym->name))
        throw std::runtime_error("The name " + sym->name + " is not a 64 bit register.");

    comp.mov(Registers::reg64[sym->name], std::dynamic_pointer_cast<Integer>(args[1])->value);

    return std::shared_ptr<Nil>();
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> Syscall::eval_body(
    const std::vector<std::shared_ptr<Object>>& args,
    Compiler& comp,
    lexical_environment& lex_env [[maybe_unused]]) const
{
    if (!args.empty())
        throw std::runtime_error("Expected zero arguments.");

    comp.syscall();

    // std::stringstream instr;
    // instr << "syscall\n";

    // comp.add_code(instr.str());

    return std::shared_ptr<Nil>();
}
