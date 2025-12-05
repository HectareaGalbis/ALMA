
#pragma once

#include "object.hpp"
#include <string>

class String : public Object {
private:
    std::string content;

    String(const std::string& content);

    virtual std::string to_string() const override;
    virtual bool typep(const Symbol& sym) const override;
};
