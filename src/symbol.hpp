
#pragma once

#include "object.hpp"
#include <optional>
#include <string>
#include <vector>

class Package;

class Symbol : public Object {
private:
    std::string name;
    std::unordered_map<ObjectTrackedRef<Symbol>, std::vector<ObjectTrackedRef<Object>>,
        ObjectRefHash, ObjectRefEqual>
        properties;

public:
    Symbol(Alma& alma, const std::string& name);

    virtual ObjectWeakRef<Object> eval(ObjectWeakRef<Object> self, ObjectWeakRef<Environment> enviroment)
        override;
    virtual std::string to_string(ObjectWeakRef<Object> self) override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;

    std::string& get_name();
    const std::string& get_name() const;

    bool has_property(ObjectWeakRef<Symbol> property);
    ObjectWeakRef<Object> get_property(ObjectWeakRef<Symbol> property);
    void set_property(ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
    void push_property(ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
    void pop_property(ObjectWeakRef<Symbol> property);

    bool has_value();
    ObjectWeakRef<Object> get_value();
    void set_value(ObjectWeakRef<Object> value);
    void push_value(ObjectWeakRef<Object> value);
    void pop_value();

    bool has_package();
    ObjectWeakRef<Package> get_package();
    void set_package(ObjectWeakRef<Package> package);
    void push_package(ObjectWeakRef<Package> package);
    void pop_package();
};

// void intern_symbols();
