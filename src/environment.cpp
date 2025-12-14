
#include "environment.hpp"
#include <stdexcept>

Environment::EnvironmentLayer::EnvironmentLayer(const EnvironmentLayer& other)
{
    for (auto& [key, value] : other.values)
        this->values.emplace(std::piecewise_construct,
            std::forward_as_tuple(*this, key),
            std::forward_as_tuple(*this, value));
}

bool Environment::EnvironmentLayer::isSymbolBound(ObjectWeakRef<Symbol> symbol) const
{
    return this->values.contains(symbol);
}

void Environment::EnvironmentLayer::insert(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> value)
{
    if (this->isSymbolBound(symbol))
        throw std::runtime_error("The symbol " + symbol->get_name() + " is already bound.");
    this->values.try_emplace(ObjectRef<Symbol>(*this, symbol), *this, value);
}

void Environment::EnvironmentLayer::setValue(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> value)
{
    if (!this->isSymbolBound(symbol))
        throw std::runtime_error("The symbol " + symbol->get_name() + " is not bound");
    this->values.find(symbol)->second = value;
}

void Environment::EnvironmentLayer::insert_or_set(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> value)
{
    if (!this->isSymbolBound(symbol))
        this->values.try_emplace(ObjectRef<Symbol>(*this, symbol), *this, value);
    else
        this->values.find(symbol)->second = value;
}

ObjectWeakRef<Symbol> Environment::EnvironmentLayer::getValue(ObjectWeakRef<Symbol> symbol) const
{
    if (!this->isSymbolBound(symbol))
        throw std::runtime_error("The symbol " + symbol->get_name() + " is not bound");
    return this->values.find(symbol)->second;
}

// --------------------------------------------------------------------------------

Environment::Environment(const Environment& other)
{
    for (auto& environmentLayer : other.values)
        this->values.emplace_back(*this, environmentLayer);
}

bool Environment::isSymbolBound(ObjectWeakRef<Symbol> symbol) const
{
    size_t len = this->values.size();
    for (size_t i = 0; i < len; i++) {
        if (this->values[len - i - 1]->isSymbolBound(symbol))
            return true;
    }
    return false;
}

void Environment::popValues()
{
    if (values.empty())
        throw std::runtime_error("The lexical environment is empty.");
    values.pop_back();
}

ObjectWeakRef<Object> Environment::getValue(ObjectWeakRef<Symbol> symbol) const
{
    size_t len = this->values.size();
    for (size_t i = 0; i < len; i++) {
        if (values[len - i - 1]->isSymbolBound(symbol))
            return values[len - i - 1]->getValue(symbol);
    }

    throw std::runtime_error("The symbol " + symbol->get_name() + " is not bound");
}

void Environment::setValue(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value)
{
    size_t len = this->values.size();
    for (size_t i = 0; i < len; i++) {
        if (this->values[len - i - 1]->isSymbolBound(symbol)) {
            this->values[len - i - 1]->setValue(symbol, value);
            return;
        }
    }
    throw std::runtime_error("The symbol " + symbol->get_name() + " is not bound");
}
