
#pragma once

#include "garbage-collector.hpp"
#include "object.hpp"
#include <filesystem>

class Environment;
class Package;
class Symbol;
class Object;

class Alma {
    friend class Object;

private:
    GarbageCollector gc;
    ObjectProtectedRef<Environment> environment;
    ObjectProtectedRef<Package> alma_package;
    ObjectProtectedRef<Package> current_package;

private:
    void intern_functions();
    void intern_symbols();

public:
    Alma();

    /** Allocates an object */
    template <typename T, typename... AS>
    ObjectWeakRef<T> make(AS&&... as);

    /** Evaluates an object */
    ObjectWeakRef<Object> eval(ObjectWeakRef<Object> obj, ObjectWeakRef<Environment> environment);
    ObjectWeakRef<Object> eval(ObjectWeakRef<Object> obj);

    /** Loads a file */
    void load(const std::filesystem::path& path);

    /** Applies an object */
    ObjectWeakRef<Object> apply(
        ObjectWeakRef<Object> obj,
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> environment);
    ObjectWeakRef<Object> apply(
        ObjectWeakRef<Object> obj,
        ObjectWeakRef<Cons> args,
        ObjectWeakRef<Environment> environment);

    /** Return the current package */
    ObjectWeakRef<Package> get_current_package();

    /** Return a string representation of an object */
    std::string to_string(ObjectWeakRef<Object> obj);

    /** Check the type of an object */
    bool typep(ObjectWeakRef<Object> obj, ObjectWeakRef<Object> sym);
    bool alma_typep(ObjectWeakRef<Object> obj, const std::string& type);

    /** Check if an object is a symbol */
    bool symbolp(ObjectWeakRef<Object> obj);

    /** Get the value of a symbol */
    ObjectWeakRef<Object> symbol_value(ObjectWeakRef<Symbol> symbol);

    /** Set the value of a symbol */
    ObjectWeakRef<Object> set_symbol_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);

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

    /** Assign a value to a symbol */
    ObjectWeakRef<Object> setq(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value,
        ObjectWeakRef<Environment> environment);
    ObjectWeakRef<Object> setq(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);

    /** Return a symbol */
    ObjectWeakRef<Object> find_symbol(const std::string& name, ObjectWeakRef<Package> package);

    /** Return a symbol from the current packge */
    ObjectWeakRef<Object> find_symbol(const std::string& name);

    /** Return a symbol from the alma package */
    ObjectWeakRef<Object> find_alma_symbol(const std::string& name);

    /** Intern a symbol */
    ObjectWeakRef<Object> intern_symbol(const std::string& name, ObjectWeakRef<Package> package);

    /** Intern a symbol in the current package */
    ObjectWeakRef<Object> intern_symbol(const std::string& name);

    /** Intern a symbol in the alma package */
    ObjectWeakRef<Object> intern_alma_symbol(const std::string& name);

    /** Return each element of a cons */
    ObjectWeakRef<Object> car(ObjectWeakRef<Cons> c);
    ObjectWeakRef<Object> cdr(ObjectWeakRef<Cons> c);
};

template <typename T, typename... AS>
ObjectWeakRef<T> Alma::make(AS&&... as)
{
    return ObjectWeakRef<T>(*this, this->gc.make_object<T>(*this, std::forward<AS>(as)...));
}
