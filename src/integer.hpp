
#pragma once

#include "object.hpp"

// Integer
class Integer : public Object {
private:
    int64_t value;

public:
    Integer(int64_t _value);

    virtual GCObjectRef eval(GCObjectRef obj, Environment& lex_env) const override;
    virtual std::string to_string() const override;
    virtual bool typep(const std::shared_ptr<Symbol>& sym) const override;

    int64_t operator*() const;
};
