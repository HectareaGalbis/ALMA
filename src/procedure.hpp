
#pragma once

#include "object.hpp"
#include "symbol.hpp"
#include <optional>
#include <vector>

class Environment;

// -----------------------------------------------------------------------------

class Procedure : public Object {
protected:
    virtual ObjectWeakRef<Object> eval_body(
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> enviroment)
        = 0;

    void check_types(
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        const std::vector<std::string>& alma_types);
    void check_types(
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        const std::vector<std::string>& alma_types,
        const std::string& alma_rest_type);

public:
    Procedure(Alma& alma);

    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;
    virtual ObjectWeakRef<Object> apply(
        ObjectWeakRef<Object> self,
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> enviroment) override;
};

// -----------------------------------------------------------------------------

class Function : public Procedure {
public:
    Function(Alma& alma);
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;
    virtual ObjectWeakRef<Object> apply(
        ObjectWeakRef<Object> self,
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> enviroment) override;
};

// -----------------------------------------------------------------------------

class Macro : public Procedure {
public:
    Macro(Alma& alma);
    ObjectWeakRef<Object> expand(
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> enviroment);
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;
    virtual ObjectWeakRef<Object> apply(
        ObjectWeakRef<Object> self,
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> enviroment) override;
};

// -----------------------------------------------------------------------------

class FunctionUser : public Function {
private:
    std::vector<ObjectTrackedRef<Symbol>> param_list;
    std::optional<ObjectRef<Symbol>> param_rest;
    mutable ObjectRef<Environment> closure;
    std::vector<ObjectTrackedRef<Object>> body;

protected:
    virtual ObjectWeakRef<Object> eval_body(
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> enviroment) override;

public:
    FunctionUser(
        Alma& alma,
        const std::vector<ObjectWeakRef<Object>>& param_list,
        const std::optional<ObjectWeakRef<Object>>& param_rest,
        ObjectWeakRef<Environment> closure,
        const std::vector<ObjectWeakRef<Object>>& body);

    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;
};

// -----------------------------------------------------------------------------

class MacroUser : public Macro {
private:
    std::vector<ObjectTrackedRef<Symbol>> param_list;
    std::optional<ObjectRef<Symbol>> param_rest;
    mutable ObjectRef<Environment> closure;
    std::vector<ObjectTrackedRef<Object>> body;

protected:
    virtual ObjectWeakRef<Object> eval_body(
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> enviroment) override;

public:
    MacroUser(
        Alma& alma,
        const std::vector<ObjectWeakRef<Object>>& param_list,
        const std::optional<ObjectWeakRef<Object>>& param_rest,
        ObjectWeakRef<Environment> closure,
        const std::vector<ObjectWeakRef<Object>>& body);

    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;
};

// -----------------------------------------------------------------------------
