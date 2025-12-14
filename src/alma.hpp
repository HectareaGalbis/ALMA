
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
    /** Evaluates an object */
    ObjectWeakRef<Object> eval(ObjectWeakRef<Object> obj);

    /** Return a string representation of an object */
    std::string to_string(ObjectWeakRef<Object> obj);

    /** Check the type of an object */
    bool typep(ObjectWeakRef<Object> obj, ObjectWeakRef<Object> sym);

    /** Check if an object is a symbol */
    bool symbolp(ObjectWeakRef<Object> obj);

    /** Check if an object is a cons */
    bool consp(ObjectWeakRef<Object> obj);

    /** Return false for the nil object and true otherwise */
    bool truep(ObjectWeakRef<Object> obj);

    /** Return true for the nil object and false otherwise */
    bool null(ObjectWeakRef<Object> obj);

    /** Return the alma's boolean objects */
    ObjectWeakRef<Object> boolean(bool v);

    /** Check if two objects are the same */
    bool eq(ObjectWeakRef<Object> obj1, ObjectWeakRef<Object> obj2);

    /** Return a symbol in the current package */
    ObjectWeakRef<Object> find_symbol(const std::string& name);

    /** Return a symbol from the given package */
    ObjectWeakRef<Object> find_symbol(const std::string& name, ObjectWeakRef<Package> package);

    /** Return a symbol in the alma package */
    ObjectWeakRef<Object> find_alma_symbol(const std::string& name);
};
