
#include "object.hpp"
#include "alma.hpp"
#include "package.hpp"
#include "util.hpp"
#include <iostream>
#include <optional>

ObjectWeakRef<> Object::eval(ObjectWeakRef<> self, Alma& alma [[maybe_unused]])
{
    return self;
}

std::string Object::to_string(ObjectWeakRef<> self [[maybe_unused]], Alma& alma [[maybe_unused]])
{
    std::stringstream ss;
    ss << "<" << this << ">";
    return ss.str();
}

bool Object::typep(ObjectWeakRef<> self [[maybe_unused]], ObjectWeakRef<> type, Alma& alma)
{
    return type == alma.alma_package->find_symbol("t", alma);
}
