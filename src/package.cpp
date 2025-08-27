
#include "package.hpp"

std::optional<std::shared_ptr<Symbol>> packages::find_symbol(const std::string& name)
{
    if (this->symbols.contains(name)) {
        return this->symbols[name];
    } else {
        return std::nullopt;
    }
}

std::shared_ptr<Symbol>& packages::intern_symbol(const std::string& name)
{
    if (!this->symbols.contains(name))
        this->symbols.try_emplace(name, std::make_shared<Symbol>(name));
    return this->symbols[name];
}

packages package;
