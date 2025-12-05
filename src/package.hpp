
#pragma once

#include "object.hpp"
#include <map>

class Package : public Object {
private:
    std::map<std::string, ObjectRef<Object>> symbols;

public:
    ObjectWeakRef<Object> find_symbol(const std::string& name, Alma& alma);
    ObjectWeakRef<Object> intern_symbol(const std::string& name, Alma& alma);

public:
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) override;
};
