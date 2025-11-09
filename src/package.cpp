
#include "package.hpp"
#include <iostream>

std::shared_ptr<Package> Package::almaPackage;
std::shared_ptr<Package> Package::currentPackage;

void Package::initAlmaPackage()
{
    Package::almaPackage = std::make_shared<Package>();
    Package::currentPackage = Package::almaPackage;
}

std::optional<std::shared_ptr<Symbol>> Package::find_symbol(const std::string& name)
{
    if (this->symbols.contains(name)) {
        return this->symbols[name];
    } else {
        return std::nullopt;
    }
}

std::shared_ptr<Symbol>& Package::intern_symbol(const std::string& name)
{
    if (!this->symbols.contains(name))
        this->symbols.try_emplace(name, std::make_shared<Symbol>(name));
    return this->symbols[name];
}

void Package::emit_impl() const
{
    throw std::runtime_error("A package cannot be emitted");
}

std::shared_ptr<Object> Package::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

std::string Package::to_string_impl() const
{
    return "<package>";
}

bool Package::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "package";
}
