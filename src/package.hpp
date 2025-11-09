
#pragma once

#include "objects.hpp"
#include <map>
#include <optional>

class Package : public Object {
public:
    static std::shared_ptr<Package> almaPackage;
    static std::shared_ptr<Package> currentPackage;

    static void initAlmaPackage();

private:
    std::map<std::string, std::shared_ptr<Symbol>> symbols;

public:
    std::optional<std::shared_ptr<Symbol>> find_symbol(const std::string& name);
    std::shared_ptr<Symbol>& intern_symbol(const std::string& name);

public:
    virtual std::shared_ptr<Object> eval_impl(
        const std::shared_ptr<Object>& obj, Environment& lex_env) const override;
    virtual void emit_impl() const override;
    virtual std::string to_string_impl() const override;
    virtual bool typep_impl(const std::shared_ptr<Symbol>& sym) const override;
};
