

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

// GarbageCollector& Alma::getGc()
// {
//     return this->gc;
// }

// Environment& Alma::getEnvironment()
// {
//     return this->environment;
// }

// GCObjectRef Alma::eval(Alma& alma, GCObjectRef obj)
// {
//     return obj.as<Object>().eval(alma, obj);
// }

// ObjectRef Alma::eq(Alma& alma, const ObjectRef& obj1, const ObjectRef& obj2)
// {
//     ObjectRef(*this, alma.gc.make_object<>);
//     return ObjectRef(*this, );
// }
