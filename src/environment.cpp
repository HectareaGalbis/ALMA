
#include "environment.hpp"
#include "objects.hpp"

bool Environment::EnvironmentLayer::isSymbolBound(const std::shared_ptr<Symbol>& symbol) const
{
    return this->values.contains(symbol) && values.at(symbol);
}

void Environment::EnvironmentLayer::setValue(const std::shared_ptr<Symbol>& symbol,
    const std::shared_ptr<Object>& value)
{
    this->values[symbol] = value;
}

std::shared_ptr<Object> Environment::EnvironmentLayer::getValue(const std::shared_ptr<Symbol>& symbol) const
{
    if (!this->isSymbolBound(symbol))
        throw std::runtime_error("The symbol " + symbol->name + " is not bound");
    return this->values.at(symbol);
}

bool Environment::isSymbolBound(const std::shared_ptr<Symbol>& symbol) const
{
    size_t len = this->values.size();
    for (size_t i = 0; i < len; i++) {
        if (this->values[len - i - 1]->isSymbolBound(symbol))
            return true;
    }
    return false;
}

void Environment::pushValues(
    const std::vector<std::shared_ptr<Symbol>>& _symbols,
    const std::vector<std::shared_ptr<Object>>& _values)
{
    if (_symbols.size() != _values.size())
        throw std::runtime_error("Symbols and values must have the same size");

    std::shared_ptr<EnvironmentLayer> layer = std::make_shared<EnvironmentLayer>();
    for (size_t i = 0; i < _symbols.size(); i++) {
        layer->setValue(_symbols[i], _values[i]);
    }

    this->values.push_back(layer);
}

void Environment::popValues()
{
    if (values.empty())
        throw std::runtime_error("The lexical environment is empty.");
    values.pop_back();
}

std::shared_ptr<Object> Environment::getValue(const std::shared_ptr<Symbol>& symbol) const
{
    size_t len = this->values.size();
    for (size_t i = 0; i < len; i++) {
        if (values[len - i - 1]->isSymbolBound(symbol))
            return values[len - i - 1]->getValue(symbol);
    }

    throw std::runtime_error("The symbol " + symbol->name + " is not bound");
}

void Environment::setValue(const std::shared_ptr<Symbol>& symbol, const std::shared_ptr<Object>& value)
{
    size_t len = this->values.size();
    for (size_t i = 0; i < len; i++) {
        if (this->values[len - i - 1]->isSymbolBound(symbol)) {
            this->values[len - i - 1]->setValue(symbol, value);
            return;
        }
    }
    throw std::runtime_error("The symbol " + symbol->name + " is not bound");
}
