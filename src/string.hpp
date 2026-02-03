
#pragma once

#include "object.hpp"
#include <string>

class String : public Object {
private:
    std::string content;

public:
    String(Alma& alma, const std::string& content);

    virtual std::string to_string(ObjectWeakRef<Object> self) override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;
};
