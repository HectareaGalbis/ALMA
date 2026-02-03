
#pragma once

#include "object.hpp"
#include "symbol.hpp"
#include <map>

class Package : public Object {
private:
    std::map<std::string, ObjectRef<Symbol>> symbols;

public:
    Package(Alma& alma);
    ObjectWeakRef<Object> find_symbol(const std::string& name);
    ObjectWeakRef<Symbol> intern_symbol(const std::string& name);
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;
};
