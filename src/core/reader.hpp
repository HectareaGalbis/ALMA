
#pragma once

#include "object.hpp"
#include <optional>

namespace ALMA::core {

class Reader : Object {
public:
    virtual std::optional<ObjectRef<Object>> read(bool eof = true) = 0;
};

}
