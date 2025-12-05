
#pragma once

#include "garbage-collector.hpp"
#include "object.hpp"

class Environment;
class Package;

class Alma {
public:
    GarbageCollector gc;
    ObjectWeakRef<Environment> environment;
    ObjectWeakRef<Package> alma_package;
    ObjectWeakRef<Package> current_package;

private:
    ObjectWeakRef<Object> last_object;

public:
    Alma();
    // GarbageCollector& getGc();
    // Environment& getEnvironment();

public:
    // static GCObjectRef<> eval(Alma& alma, GCObjectRef obj);
    // static ObjectRef eq(const ObjectRef& obj1, const ObjectRef& obj2);
};
