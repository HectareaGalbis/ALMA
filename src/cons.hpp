
#pragma once

#include "object.hpp"

class Cons : public Object {
private:
    std::shared_ptr<Object> car;
    std::shared_ptr<Object> cdr;

public:
    Cons(const std::shared_ptr<Object>& _car, const std::shared_ptr<Object>& _cdr);
    Cons(const std::vector<std::shared_ptr<Object>>& list);

    std::vector<std::shared_ptr<Object>> toList() const;

    virtual std::shared_ptr<Object> eval(
        const std::shared_ptr<Object>& obj, Environment& lex_env) const override;
    virtual void emit_impl() const override;
    virtual std::string to_string() const override;
    virtual bool typep(const std::shared_ptr<Symbol>& sym) const override;
};
