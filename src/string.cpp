
#include "string.hpp"

String::String(const std::string& _content)
    : content(_content)
{
}

std::shared_ptr<Object> String::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

void String::emit_impl() const
{
    Emitter::emit(this->content);
}

std::string String::to_string_impl() const
{
    return this->content;
}

bool String::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "string";
}
