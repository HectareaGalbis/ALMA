
#pragma once

#include "alma.hpp"
#include "object.hpp"
#include "symbol.hpp"
#include <stdexcept>
#include <unordered_map>
#include <vector>

class Environment : public Object {
    friend class EnvironmentLayer;

private:
    class EnvironmentLayer {
    private:
        class EnvironmentProperty {
        private:
            Environment& owner;
            std::unordered_map<ObjectRef<Symbol>, ObjectRef<Object>, ObjectRefHash, ObjectRefEqual> values;

        public:
            EnvironmentProperty(Environment& owner);
            EnvironmentProperty(Environment& owner, const EnvironmentProperty& other);
            bool has_symbol(ObjectWeakRef<Symbol> symbol) const;
            void insert(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);
            void set_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);
            void insert_or_set_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);
            ObjectWeakRef<Object> get_value(ObjectWeakRef<Symbol> symbol) const;
        };

    private:
        Environment& owner;
        std::unordered_map<ObjectRef<Symbol>, EnvironmentProperty, ObjectRefHash, ObjectRefEqual> properties;

    public:
        EnvironmentLayer(Environment& owner);
        EnvironmentLayer(Environment& owner, const EnvironmentLayer& other);
        bool has_property(ObjectWeakRef<Symbol> property) const;
        bool has_symbol_property(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
        void insert(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
        void set_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
        void insert_or_set_value(
            ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
        ObjectWeakRef<Object> get_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
    };

private:
    std::vector<EnvironmentLayer> layers;

private:
    void push_layer();
    void pop_layer();
    bool has_symbol_property(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
    void insert_or_set_value(
        ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
    void set_value(
        ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
    ObjectWeakRef<Object> get_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;

public:
    Environment(const Environment& other);
};

class EnvironmentLayer {
private:
    Environment& environment;

public:
    EnvironmentLayer(Environment& environment);
    ~EnvironmentLayer();
    bool has_symbol_property(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
    void insert_or_set_value(
        ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
    void set_value(
        ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
    ObjectWeakRef<Object> get_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
};
