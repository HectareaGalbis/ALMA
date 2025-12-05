
#include "integer.hpp"

Integer::Integer(int64_t _value)
    : value(_value)
{
}

std::shared_ptr<Object> Integer::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

void Integer::emit_impl() const
{
    Emitter::emit(this->value);
}

std::string Integer::to_string_impl() const
{
    return std::to_string(this->value);
}

bool Integer::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "integer";
}

GCObjectRef Integer::eval(GCObjectRef obj, Environment& lex_env) const
{
    return obj;
}

int64_t Integer::operator*() const
{
    return this->value;
}
