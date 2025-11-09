
#pragma once

#include <map>
#include <memory>
#include <vector>

struct Symbol;
class Object;

class Environment {
private:
    class EnvironmentLayer {
    private:
        std::map<std::shared_ptr<Symbol>, std::shared_ptr<Object>> values;

    public:
        template <typename InputIt>
        void insert(InputIt start, InputIt end);
        bool isSymbolBound(const std::shared_ptr<Symbol>& symbol) const;
        void setValue(const std::shared_ptr<Symbol>& symbol, const std::shared_ptr<Object>& value);
        std::shared_ptr<Object> getValue(const std::shared_ptr<Symbol>& symbol) const;
    };

private:
    std::vector<std::shared_ptr<EnvironmentLayer>> values;

public:
    bool isSymbolBound(const std::shared_ptr<Symbol>& symbol) const;
    template <typename InputIt>
    void pushValues(InputIt first, InputIt second);
    void pushValues(
        const std::vector<std::shared_ptr<Symbol>>& symbols,
        const std::vector<std::shared_ptr<Object>>& values);
    void popValues();
    std::shared_ptr<Object> getValue(const std::shared_ptr<Symbol>& symbol) const;
    void setValue(const std::shared_ptr<Symbol>& symbol, const std::shared_ptr<Object>& value);
};

template <typename InputIt>
void Environment::EnvironmentLayer::insert(InputIt start, InputIt end)
{
    this->values.insert(start, end);
}

template <typename InputIt>
void Environment::pushValues(InputIt first, InputIt second)
{
    std::shared_ptr<EnvironmentLayer> layer = std::make_shared<EnvironmentLayer>();
    layer->insert(first, second);
    this->values.push_back(layer);
}
