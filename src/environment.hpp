
#pragma once

#include "alma.hpp"
#include "object.hpp"
#include "symbol.hpp"
#include <stdexcept>
#include <unordered_map>
#include <vector>

class Environment : public Object {
private:
    class EnvironmentLayer : public Object {
    private:
        std::unordered_map<ObjectRef<Symbol>, ObjectRef<Object>, ObjectRefHash, ObjectRefEqual> values;

    public:
        EnvironmentLayer(const EnvironmentLayer& other);
        bool isSymbolBound(ObjectWeakRef<Symbol> symbol) const;
        template <typename InputIt>
        void insert(InputIt start, InputIt end);
        void insert(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> value);
        void setValue(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> value);
        void insert_or_set(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> value);
        ObjectWeakRef<Symbol> getValue(ObjectWeakRef<Symbol> symbol) const;
    };

private:
    std::vector<ObjectRef<EnvironmentLayer>> values;

public:
    Environment(const Environment& other);
    bool isSymbolBound(ObjectWeakRef<Symbol> symbol) const;
    template <typename InputIt>
    void pushValues(InputIt first, InputIt second, Alma& alma);
    template <ObjectRefType<Symbol> S, ObjectRefType<Object> O>
    void pushValues(const std::vector<S>& symbols, const std::vector<O>& values, Alma& alma);
    void popValues();
    ObjectWeakRef<Object> getValue(ObjectWeakRef<Symbol> symbol) const;
    void setValue(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);
};

template <typename InputIt>
void Environment::EnvironmentLayer::insert(InputIt start, InputIt end)
{
    this->values.insert(start, end);
}

template <typename InputIt>
void Environment::pushValues(InputIt first, InputIt second, Alma& alma)
{
    ObjectWeakRef<EnvironmentLayer> layer = alma.gc.make_object<EnvironmentLayer>();
    layer->insert(first, second);
    this->values.emplace_back(*this, layer);
}

template <ObjectRefType<Symbol> S, ObjectRefType<Object> O>
void Environment::pushValuess(const std::vector<S>& _symbols, const std::vector<O>& _values, Alma& alma)
{
    if (_symbols.size() != _values.size())
        throw std::runtime_error("Symbols and values must have the same size");

    ObjectWeakRef<EnvironmentLayer> layer = alma.gc.make_object<EnvironmentLayer>();
    for (size_t i = 0; i < _symbols.size(); i++) {
        layer->insert_or_set(_symbols[i], _values[i]);
    }
    this->values.emplace_back(*this, layer);
}
