
#pragma once

#include "object.hpp"
#include "symbol.hpp"
#include <map>

class Package : public Object {
private:
    std::map<std::string, ObjectTrackedRef<Symbol>> symbols;

public:
    Package(Alma& alma);
    ObjectRef<Object> find_symbol(const std::string& name);
    ObjectRef<Symbol> intern_symbol(const std::string& name);
    virtual bool typep(ObjectRef<Object> self, ObjectRef<Object> type) override;
};
