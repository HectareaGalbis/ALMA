
#pragma once

#include "alma.hpp"
#include "object.hpp"
#include "symbol.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

class Environment : public Object {
    friend class EnvironmentLayer;

private:
    class Layer {
    private:
        class Property {
        private:
            Environment& owner;
            std::unordered_map<ObjectTrackedRef<Symbol>, ObjectRef<Object>, ObjectRefHash, ObjectRefEqual> values;

        public:
            Property(Environment& owner);
            Property(Environment& owner, const Property& other);
            bool has_symbol(ObjectWeakRef<Symbol> symbol) const;
            void insert(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);
            void set_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);
            void insert_or_set_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value);
            std::optional<ObjectWeakRef<Object>> get_value(ObjectWeakRef<Symbol> symbol) const;
        };

    private:
        Environment& owner;
        std::unordered_map<ObjectRef<Symbol>, Property, ObjectRefHash, ObjectRefEqual> properties;

    public:
        Layer(Environment& owner);
        Layer(Environment& owner, const Layer& other);
        bool has_property(ObjectWeakRef<Symbol> property) const;
        bool has_symbol_property(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
        void insert(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
        void set_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
        void insert_or_set_value(
            ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
        std::optional<ObjectWeakRef<Object>> get_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
    };

private:
    std::vector<Layer> layers;

public:
    class WithLayer {
    private:
        Environment& environment;

    public:
        WithLayer(Environment& environment);
        ~WithLayer();

        static void* operator new(std::size_t) = delete;
        static void* operator new[](std::size_t) = delete;
    };

public:
    Environment(Alma& alma);
    Environment(const Environment& other);

    bool has_symbol_property(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
    void insert_or_set_value(
        ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
    void set_value(
        ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value);
    std::optional<ObjectWeakRef<Object>> get_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Symbol> property) const;
};
