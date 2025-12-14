
#pragma once

#include "object.hpp"
#include <optional>
#include <string>
#include <vector>

class Procedure;
class Package;

class Symbol : public Object {
private:
    std::string name;
    // std::unordered_map<ObjectRef, std::vector<ObjectRef>> properties;
    std::vector<ObjectRef<Object>> values;
    std::vector<ObjectRef<Procedure>> functions;
    std::vector<ObjectRef<Package>> packages;

public:
    Symbol(const std::string& _name);

    virtual ObjectWeakRef<Object> eval(ObjectWeakRef<Object> self, Alma& alma) const override;
    virtual std::string to_string(ObjectWeakRef<Object> self, Alma& alma) const override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const override;

    std::string& get_name();
    const std::string& get_name() const;

    std::optional<ObjectWeakRef<Object>> get_value();
    void set_value(ObjectWeakRef<Object> value);
    void push_value(ObjectWeakRef<Object> value);
    void pop_value();

    std::optional<ObjectWeakRef<Procedure>> get_function();
    void set_function(ObjectWeakRef<Procedure> procedure);
    void push_function(ObjectWeakRef<Procedure> procedure);
    void pop_function();

    std::optional<ObjectWeakRef<Package>> get_package();
    void set_package(ObjectWeakRef<Package> package);
    void push_package(ObjectWeakRef<Package> package);
    void pop_package();
};

void intern_symbols();
