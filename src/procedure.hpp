
#pragma once

#include "object.hpp"
#include <string>

class Environment;
class Symbol;
class Cons;

class Procedure : public Object {
public:
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const override;

    virtual ObjectWeakRef<Object> apply(ObjectWeakRef<Cons> arguments, Alma& alma) = 0;
};

struct Function : Procedure {
private:
    static std::vector<ObjectWeakRef<Object>> eval_args(
        const std::vector<ObjectWeakRef<Object>>& args,
        Alma& alma);

protected:
    virtual ObjectWeakRef<Object> eval_body(const std::vector<ObjectWeakRef<Object>>& args, Alma& alma) = 0;

public:
    virtual ObjectWeakRef<Object> apply(ObjectWeakRef<Cons> arguments, Alma& alma) override;

    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const override;
};

struct FunctionUser : Function {
    ObjectRef<Environment> closure;
    std::vector<ObjectRef<Symbol>> params;
    std::vector<ObjectRef<Object>> body;

    FunctionUser(ObjectWeakRef<Environment> closure,
        const std::vector<ObjectWeakRef<Symbol>>& params,
        const std::vector<ObjectWeakRef<Object>>& body);

protected:
    virtual ObjectWeakRef<Object> eval_body(const std::vector<ObjectWeakRef<Object>>& args, Alma& alma) override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const override;
};

struct Macro : Procedure {
    template <typename Name>
    Macro(Name&& _name)
        : Procedure(std::forward<Name>(_name))
    {
    }

protected:
    virtual ObjectWeakRef<Object> eval_body(const std::vector<ObjectWeakRef<Object>>& args, Alma& alma) = 0;

public:
    virtual ObjectWeakRef<Object> apply(
        const std::vector<ObjectWeakRef<Object>>& arguments, Alma& alma) override;
    ObjectWeakRef<Object> expand(const std::vector<ObjectWeakRef<Object>>& arguments, Alma& alma);

    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const override;
};

struct MacroUser : Macro {
    ObjectRef<Environment> closure;
    std::vector<ObjectRef<Symbol>> params;
    std::vector<ObjectRef<Object>> body;

    MacroUser(ObjectWeakRef<Environment> closure,
        const std::vector<ObjectWeakRef<Symbol>>& params,
        const std::vector<ObjectWeakRef<Object>>& body);

protected:
    virtual ObjectWeakRef<Object> eval_body(const std::vector<ObjectWeakRef<Object>>& args, Alma& alma) override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const override;
};
