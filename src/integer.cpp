
#include "integer.hpp"
#include "alma.hpp"

Integer::Integer(Alma& _alma, int64_t _value)
    : Object(_alma)
    , value(_value)
{
}

std::string Integer::to_string(ObjectWeakRef<Object> self [[maybe_unused]])
{
    return std::to_string(this->value);
}

bool Integer::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("integer") || this->Object::typep(self, type);
}

int64_t Integer::operator*() const
{
    return this->value;
}
