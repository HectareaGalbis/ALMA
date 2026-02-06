
#include "object.hpp"
#include "alma.hpp"
#include "debug.hpp"
#include "package.hpp"
#include "util.hpp"
#include <iostream>
#include <optional>

void Object::protect_object(Alma& alma, GCObject** object)
{
    alma.gc.track_root_object(object);
}

void Object::unprotect_object(Alma& alma, GCObject** object)
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

ObjectRef<Object> Object::eval(
    ObjectRef<Object> self, ObjectRef<Environment> environment [[maybe_unused]])
{
    return self;
}

ObjectRef<Object> Object::apply(
    ObjectRef<Object> self,
    const std::vector<ObjectRef<Object>>& arg_list [[maybe_unused]],
    ObjectRef<Environment> enviroment [[maybe_unused]])
{
    athrow("The object " << this->to_string(self) << " is not applicable");
}

std::string Object::to_string(ObjectRef<Object> self [[maybe_unused]])
{
    std::stringstream ss;
    ss << "<" << this << ">";
    return ss.str();
}

std::string Object::to_string()
{
    std::stringstream ss;
    ss << "<object>";
    return ss.str();
}

bool Object::typep(ObjectRef<Object> self [[maybe_unused]], ObjectRef<Object> type)
{
    return type == this->alma.intern_alma_symbol("t");
}

Object::operator bool()
{
    return this != this->alma.intern_alma_symbol("nil").get();
}
