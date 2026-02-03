
#include "function.hpp"
#include "alma.hpp"
#include "cons.hpp"
#include "debug.hpp"
#include "integer.hpp"
#include "object.hpp"
#include "package.hpp"
#include <iostream>

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Sum::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    this->check_types(arg_list, {}, "integer");
    int64_t sum_value = 0;
    for (ObjectWeakRef<Object> arg : arg_list) {
        sum_value += *arg.as<Integer>();
    }
    return this->alma.make<Integer>(sum_value);
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Print::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    this->check_types(arg_list, { "t" });
    std::cout << this->alma.to_string(arg_list.front()) << std::endl;
    return arg_list.front();
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Typep::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    this->check_types(arg_list, { "t", "symbol" });
    bool correct_type = this->alma.typep(arg_list[0], arg_list[1]);
    return this->alma.boolean(correct_type);
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Set_symbol_value::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    this->check_types(arg_list, { "symbol", "t" });
    return this->alma.set_symbol_value(arg_list[0], arg_list[1]);
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Setq::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    this->check_types(arg_list, { "symbol", "t" });
    return this->alma.setq(arg_list[0], arg_list[1]);
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Car::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    this->check_types(arg_list, { "cons" });
    return this->alma.car(arg_list[0]);
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Cdr::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    this->check_types(arg_list, { "cons" });
    return this->alma.cdr(arg_list[0]);
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Eq::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    this->check_types(arg_list, { "t", "t" });
    return this->alma.boolean(this->alma.eq(arg_list[0], arg_list[1]));
}

// --------------------------------------------------------------------------------

ObjectWeakRef<Object> Eval::eval_body(
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> environment)
{
    this->check_types(arg_list, { "t" });

    return this->alma.eval(arg_list[0], environment);
}
