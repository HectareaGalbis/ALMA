
#pragma once

#include "object.hpp"
#include <string>
#include <vector>

class Procedure;
class Package;

class Symbol : public Object {
private:
    std::string name;
    // std::unordered_map<ObjectRef, std::vector<ObjectRef>> properties;
    std::vector<ObjectRef<Object>> values;
    ObjectRef<Procedure> function;
    ObjectRef<Package> package;

public:
    Symbol(const std::string& _name);

    virtual ObjectWeakRef<Object> eval(ObjectWeakRef<Object> self, Alma& alma) override;
    virtual std::string to_string(ObjectWeakRef<Object> self, Alma& alma) override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) override;
};

void intern_symbols();
