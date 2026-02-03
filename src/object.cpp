
#include "object.hpp"
#include "alma.hpp"
#include "debug.hpp"
#include "package.hpp"
#include "util.hpp"
#include <iostream>
#include <optional>

void Object::protect_object(Alma& alma, GCObject* object)
{
    alma.gc.track_root_object(object);
}

void Object::unprotect_object(Alma& alma, GCObject* object)
{
    alma.gc.untrack_root_object(object);
}

Object::Object(Alma& _alma)
    : alma(_alma)
{
}

Object::Object(const Object& other)
    : alma(other.alma)
{
}

Object::Object(const Object&& other)
    : alma(other.alma)
{
}

ObjectWeakRef<Object> Object::eval(
    ObjectWeakRef<Object> self, ObjectWeakRef<Environment> environment [[maybe_unused]])
{
    return self;
}

ObjectWeakRef<Object> Object::apply(
    ObjectWeakRef<Object> self,
    const std::vector<ObjectWeakRef<Object>>& arg_list [[maybe_unused]],
    ObjectWeakRef<Environment> enviroment [[maybe_unused]])
{
    mthrow("The object " << this->to_string(self) << " is not applicable");
}

std::string Object::to_string(ObjectWeakRef<Object> self [[maybe_unused]])
{
    std::stringstream ss;
    ss << "<" << this << ">";
    return ss.str();
}

bool Object::typep(ObjectWeakRef<Object> self [[maybe_unused]], ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("t");
}

Object::operator bool()
{
    return this != this->alma.find_alma_symbol("nil").get();
}
