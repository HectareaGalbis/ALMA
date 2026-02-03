
#pragma once

#include "object.hpp"

// Integer
class Integer : public Object {
private:
    int64_t value;

public:
    Integer(Alma& alma, int64_t _value);

    virtual std::string to_string(ObjectWeakRef<Object> self) override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;

    int64_t operator*() const;
};
