

#include "alma.hpp"
#include "environment.hpp"
#include "object.hpp"
#include "package.hpp"

Alma::Alma()
    : environment(gc.make_object<Environment>())
    , alma_package(gc.make_object<Package>())
    , current_package(alma_package)
    , last_object(alma_package)
{
    gc.track_root_object(this->environment.obj);
    gc.track_root_object(this->alma_package.obj);
    gc.track_root_object(this->current_package.obj);
    gc.track_root_object(this->last_object.obj);
}

ObjectWeakRef<Object> Alma::eval(ObjectWeakRef<Object> obj)
{
    return obj->eval(obj, *this);
}

std::string Alma::to_string(ObjectWeakRef<Object> obj)
{
    return obj->to_string(obj, *this);
}

bool Alma::typep(ObjectWeakRef<Object> obj, ObjectWeakRef<Object> sym)
{
    return obj->typep(obj, sym, *this);
}

bool Alma::symbolp(ObjectWeakRef<Object> obj)
{
    return this->typep(obj, this->find_alma_symbol("symbol"));
}

bool Alma::consp(ObjectWeakRef<Object> obj)
{
    return this->typep(obj, this->find_alma_symbol("cons"));
}

bool Alma::truep(ObjectWeakRef<Object> obj)
{
    return obj != this->find_alma_symbol("nil");
}

bool Alma::null(ObjectWeakRef<Object> obj)
{
    return obj == this->find_alma_symbol("nil");
}

ObjectWeakRef<Object> Alma::boolean(bool v)
{
    return this->alma_package->find_symbol(v ? "t" : "nil", *this);
}

bool Alma::eq(ObjectWeakRef<Object> obj1, ObjectWeakRef<Object> obj2)
{
    return obj1 == obj2;
}

ObjectWeakRef<Object> Alma::find_symbol(const std::string& name)
{
    return this->current_package->find_symbol(name, *this);
}

ObjectWeakRef<Object> Alma::find_symbol(const std::string& name, ObjectWeakRef<Package> package)
{
    return package->find_symbol(name, *this);
}

ObjectWeakRef<Object> Alma::find_alma_symbol(const std::string& name)
{
    return this->alma_package->find_symbol(name, *this);
}
