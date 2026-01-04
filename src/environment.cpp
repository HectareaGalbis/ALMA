
#include "environment.hpp"
#include "debug.hpp"
#include <stdexcept>

Environment::EnvironmentLayer::EnvironmentProperty::EnvironmentProperty(Environment& _owner)
    : owner(_owner)
{
}

Environment::EnvironmentLayer::EnvironmentProperty::EnvironmentProperty(
    Environment& _owner, const EnvironmentProperty& other)
    : EnvironmentProperty(_owner)
{
    for (auto& [key, value] : other.values)
        this->values.emplace(std::piecewise_construct,
            std::forward_as_tuple(this->owner, key),
            std::forward_as_tuple(this->owner, value));
}

bool Environment::EnvironmentLayer::EnvironmentProperty::has_symbol(ObjectWeakRef<Symbol> symbol) const
{
    return this->values.contains(symbol);
}

void Environment::EnvironmentLayer::EnvironmentProperty::insert(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value)
{
    if (this->has_symbol(symbol))
        mthrow("The symbol " << symbol->get_name() << " is already in the environment property");
    this->values.try_emplace(ObjectRef<Symbol>(this->owner, symbol), this->owner, value);
}

void Environment::EnvironmentLayer::EnvironmentProperty::set_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value)
{
    if (!this->has_symbol(symbol))
        mthrow("The symbol " << symbol->get_name() << " is not in the environment property");
    this->values.find(symbol)->second = value;
}

void Environment::EnvironmentLayer::EnvironmentProperty::insert_or_set_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value)
{
    if (!this->has_symbol(symbol))
        this->values.try_emplace(ObjectRef<Symbol>(this->owner, symbol), this->owner, value);
    else
        this->values.find(symbol)->second = value;
}

ObjectWeakRef<Object> Environment::EnvironmentLayer::EnvironmentProperty::get_value(
    ObjectWeakRef<Symbol> symbol) const
{
    if (!this->has_symbol(symbol))
        mthrow("The symbol " << symbol->get_name() << " is not in the environment property");
    return this->values.find(symbol)->second;
}

// --------------------------------------------------------------------------------

Environment::EnvironmentLayer::EnvironmentLayer(Environment& _owner)
    : owner(_owner)
{
}

Environment::EnvironmentLayer::EnvironmentLayer(Environment& _owner, const EnvironmentLayer& other)
    : EnvironmentLayer(_owner)
{
    for (auto& [key, value] : other.properties)
        this->properties.emplace(std::piecewise_construct,
            std::forward_as_tuple(this->owner, key),
            std::forward_as_tuple(this->owner, value));
}

bool Environment::EnvironmentLayer::has_property(ObjectWeakRef<Symbol> property) const
{
    return this->properties.contains(property);
}

bool Environment::EnvironmentLayer::has_symbol_property(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const
{
    return this->has_property(property) && this->properties.find(property)->second.has_symbol(symbol);
}

void Environment::EnvironmentLayer::insert(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    if (!this->has_property(property)) {
        this->properties.emplace(std::piecewise_construct,
            std::forward_as_tuple(this->owner, property),
            std::forward_as_tuple(this->owner));
    }
    EnvironmentProperty& env_property = this->properties.find(property)->second;
    if (env_property.has_symbol(symbol))
        mthrow("The symbol " + symbol->get_name() + " already has the property " << property->get_name());
    env_property.insert(symbol, value);
}

void Environment::EnvironmentLayer::set_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    if (!this->has_symbol_property(symbol, property))
        mthrow("The symbol " << symbol->get_name() << " has not the property " << property->get_name());
    this->properties.find(symbol)->second.set_value(symbol, value);
}

void Environment::EnvironmentLayer::insert_or_set_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    if (!this->has_property(property)) {
        this->properties.emplace(std::piecewise_construct,
            std::forward_as_tuple(this->owner, property),
            std::forward_as_tuple(this->owner));
    }
    this->properties.find(property)->second.insert_or_set_value(symbol, value);
}

ObjectWeakRef<Object> Environment::EnvironmentLayer::get_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const
{
    if (!this->has_symbol_property(symbol, property))
        mthrow("The symbol " << symbol->get_name() << " has not the property" << property->get_name());
    return this->properties.find(property)->second.get_value(symbol);
}

// --------------------------------------------------------------------------------

Environment::Environment(const Environment& other)
{
    for (const EnvironmentLayer& layer : other.layers)
        this->layers.emplace_back(*this, layer);
}

void Environment::push_layer()
{
    this->layers.emplace_back(*this);
}

void Environment::pop_layer()
{
    if (this->layers.empty())
        mthrow("The lexical environment is empty.");
    this->layers.pop_back();
}

bool Environment::has_symbol_property(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const
{
    size_t len = this->layers.size();
    for (size_t i = 0; i < len; i++) {
        if (this->layers[len - i - 1].has_symbol_property(symbol, property))
            return true;
    }
    return false;
}

void Environment::insert_or_set_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    if (this->layers.empty())
        mthrow("The lexical environment is empty.");
    this->layers.back().insert_or_set_value(symbol, property, value);
}

void Environment::set_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    size_t len = this->layers.size();
    for (size_t i = 0; i < len; i++) {
        if (this->layers[len - i - 1].has_symbol_property(symbol, property)) {
            this->layers[len - i - 1].set_value(symbol, property, value);
            return;
        }
    }
    mthrow("The symbol " << symbol->get_name() << " has not the property " << property->get_name() << " in the lexcal environment");
}

ObjectWeakRef<Object> Environment::get_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const
{
    size_t len = this->layers.size();
    for (size_t i = 0; i < len; i++) {
        if (this->layers[len - i - 1].has_symbol_property(symbol, property))
            return this->layers[len - i - 1].get_value(symbol, property);
    }
    mthrow("The symbol " + symbol->get_name() + " has not the property " << property->get_name() << " in the lexical environment");
}

// --------------------------------------------------------------------------------

EnvironmentLayer::EnvironmentLayer(Environment& _environment)
    : environment(_environment)
{
    environment.push_layer();
}

EnvironmentLayer::~EnvironmentLayer()
{
    environment.pop_layer();
}

bool EnvironmentLayer::has_symbol_property(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const
{
    return this->environment.has_symbol_property(symbol, property);
}

void EnvironmentLayer::insert_or_set_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    this->environment.insert_or_set_value(symbol, property, value);
}

void EnvironmentLayer::set_value(
    ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    this->environment.set_value(symbol, property, value);
}

ObjectWeakRef<Object> EnvironmentLayer::get_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const
{
    return this->environment.get_value(symbol, property);
}
