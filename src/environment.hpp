
#pragma once

#include <map>
#include <memory>
#include <vector>

struct Symbol;
class Object;

class lexical_environment {
private:
    std::map<std::shared_ptr<Symbol>, std::vector<std::shared_ptr<Object>>> values;

public:
    bool is_symbol_bound(const std::shared_ptr<Symbol>& symbol);

    void push_value(const std::shared_ptr<Symbol>& symbol, const std::shared_ptr<Object>& value);

    void pop_value(const std::shared_ptr<Symbol>& symbol);

    std::shared_ptr<Object> get_value(const std::shared_ptr<Symbol>& symbol);
};
