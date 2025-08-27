
#pragma once

#include "objects.hpp"
#include <map>
#include <optional>

class packages {
private:
    std::map<std::string, std::shared_ptr<Symbol>> symbols;

public:
    std::optional<std::shared_ptr<Symbol>> find_symbol(const std::string& name);
    std::shared_ptr<Symbol>& intern_symbol(const std::string& name);
};

extern packages package;
