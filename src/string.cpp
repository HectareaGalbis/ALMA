
#include "string.hpp"
#include "alma.hpp"

String::String(Alma& _alma, const std::string& _content)
    : Object(_alma)
    , content(_content)
{
}

std::string String::to_string(ObjectWeakRef<Object> self [[maybe_unused]])
{
    return this->content;
}

bool String::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("string") || this->Object::typep(self, type);
}
