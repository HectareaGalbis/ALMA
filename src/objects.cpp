
#include "objects.hpp"
#include "emitter.hpp"
#include "util.hpp"
#include <iostream>
#include <optional>

// --------------------------------------------------------------------------------

std::shared_ptr<Object> Object::eval(
    const std::shared_ptr<Object>& obj, Environment& lex_env)
{
    // debugMsg("Eval: " << Object::to_string(obj));
    std::shared_ptr<Object> r = obj->eval_impl(obj, lex_env);
    return r;
}

void Object::emit(const std::shared_ptr<Object>& obj)
{
    obj->emit_impl();
}

std::string Object::to_string(const std::shared_ptr<Object>& obj)
{
    return obj->to_string_impl();
}

bool Object::is_true(const std::shared_ptr<Object>& obj)
{
    return static_cast<bool>(*obj);
}

bool Object::eq(const std::shared_ptr<Object>& obj1, const std::shared_ptr<Object>& obj2)
{
    return obj1 == obj2;
}

bool Object::typep(const std::shared_ptr<Object>& obj, const std::shared_ptr<Symbol>& sym)
{
    return sym->name == "t" || obj->typep_impl(sym);
}

// --------------------------------------------------------------------------------

Integer::Integer(int64_t _value)
    : value(_value)
{
}

std::shared_ptr<Object> Integer::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

void Integer::emit_impl() const
{
    Emitter::emit(this->value);
}

std::string Integer::to_string_impl() const
{
    return std::to_string(this->value);
}

bool Integer::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "integer";
}

// --------------------------------------------------------------------------------

String::String(const std::string& _content)
    : content(_content)
{
}

std::shared_ptr<Object> String::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

void String::emit_impl() const
{
    Emitter::emit(this->content);
}

std::string String::to_string_impl() const
{
    return this->content;
}

bool String::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "string";
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> Procedure::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

void Procedure::emit_impl() const
{
    throw std::runtime_error("A procedure cannot be emitted");
}

std::string Procedure::to_string_impl() const
{
    std::stringstream s;
    s << "<";
    s << std::hex << this;
    if (this->name)
        s << " " << *this->name;
    s << ">";
    return s.str();
}

bool Procedure::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "procedure";
}

std::vector<std::shared_ptr<Object>> Function::eval_args(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env)
{
    std::vector<std::shared_ptr<Object>> evaluated_args;
    evaluated_args.reserve(args.size());
    for (const std::shared_ptr<Object>& arg : args) {
        evaluated_args.push_back(Object::eval(arg, lex_env));
    }

    return evaluated_args;
}

std::shared_ptr<Object> Function::apply(
    Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments)
{
    std::vector<std::shared_ptr<Object>> evaluated_args = eval_args(arguments, lex_env);
    return this->eval_body(evaluated_args, lex_env);
}

bool Function::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "function" || this->Procedure::typep_impl(sym);
}

std::shared_ptr<Object> FunctionUser::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (this->params.size() != args.size())
        throw std::runtime_error("Needed " + std::to_string(this->params.size()) + " but received " + std::to_string(args.size()) + " params");

    this->closure.pushValues(this->params, args);

    for (size_t i = 0; i < this->body.size() - 1; i++) {
        Object::eval(this->body[i], this->closure);
    }
    std::shared_ptr<Object> result = Object::eval(this->body.back(), this->closure);

    this->closure.popValues();

    return result;
}

bool FunctionUser::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "function-user" || this->Function::typep_impl(sym);
}

std::shared_ptr<Object> Macro::apply(
    Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments)
{
    std::shared_ptr<Object> result = this->eval_body(arguments, lex_env);
    return Object::eval(result, lex_env);
}

std::shared_ptr<Object> Macro::expand(Environment& lex_env, const std::vector<std::shared_ptr<Object>>& arguments)
{
    return this->eval_body(arguments, lex_env);
}

bool Macro::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "macro" || this->Procedure::typep_impl(sym);
}

std::shared_ptr<Object> MacroUser::eval_body(
    const std::vector<std::shared_ptr<Object>>& args, Environment& lex_env [[maybe_unused]])
{
    if (this->params.size() != args.size())
        throw std::runtime_error("Needed " + std::to_string(this->params.size()) + " but received " + std::to_string(args.size()) + " params");

    this->closure.pushValues(this->params, args);

    for (size_t i = 0; i < this->body.size() - 1; i++) {
        Object::eval(this->body[i], this->closure);
    }
    std::shared_ptr<Object> result = Object::eval(this->body.back(), this->closure);

    this->closure.popValues();

    return result;
}

bool MacroUser::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "macro-user" || this->Macro::typep_impl(sym);
}

// --------------------------------------------------------------------------------

Symbol::Symbol(const std::string& _name)
    : name(_name)
{
}

std::shared_ptr<Object> Symbol::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env) const
{
    std::shared_ptr<Symbol> self = std::dynamic_pointer_cast<Symbol>(obj);
    if (lex_env.isSymbolBound(self))
        return lex_env.getValue(self);
    else {
        if (this->values.empty())
            throw std::runtime_error("Symbol " + this->name + " unbound.");
        return this->values.back();
    }
}

void Symbol::emit_impl() const
{
    Emitter::emit(this->name);
}

std::string Symbol::to_string_impl() const
{
    return this->name;
}

bool Symbol::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "symbol";
}

// --------------------------------------------------------------------------------

Cons::Cons(const std::shared_ptr<Object>& _car, const std::shared_ptr<Object>& _cdr)
    : car(_car)
    , cdr(_cdr)
{
}

static std::shared_ptr<Cons> makeConsFromList(const std::vector<std::shared_ptr<Object>>& list,
    size_t currentIndex)
{
    if (currentIndex == list.size() - 1) {
        return std::make_shared<Cons>(list[currentIndex], std::make_shared<Nil>());
    } else {
        return std::make_shared<Cons>(list[currentIndex], makeConsFromList(list, currentIndex + 1));
    }
}

Cons::Cons(const std::vector<std::shared_ptr<Object>>& list)
{
    if (list.empty())
        throw std::runtime_error("The list is empty");

    std::shared_ptr<Cons> newCons = makeConsFromList(list, 0);
    this->car = newCons->car;
    this->cdr = newCons->cdr;
}

std::vector<std::shared_ptr<Object>> Cons::toList() const
{
    std::vector<std::shared_ptr<Object>> list;
    list.push_back(this->car);
    std::shared_ptr<Object> argIt = this->cdr;
    while (Object::is_true(argIt)) {
        std::shared_ptr<Cons> consIt = std::dynamic_pointer_cast<Cons>(argIt);
        if (!consIt)
            throw std::runtime_error("Error: Not a proper list.");
        list.push_back(consIt->car);
        argIt = consIt->cdr;
    }
    return list;
}

std::shared_ptr<Object> Cons::eval_impl(
    const std::shared_ptr<Object>& obj [[maybe_unused]], Environment& lex_env) const
{
    std::shared_ptr<Symbol> func_name = std::dynamic_pointer_cast<Symbol>(this->car);
    if (!func_name)
        throw std::runtime_error("Expected a symbol denoting a procedure. Found a " + Object::to_string(this->car));
    if (!func_name->function)
        throw std::runtime_error("The symbol " + func_name->name + " does not denote a procedure.");

    if (!Object::is_true(this->cdr)) {
        return func_name->function->apply(lex_env, {});
    } else {
        std::shared_ptr<Cons> arguments = std::dynamic_pointer_cast<Cons>(this->cdr);
        if (!arguments)
            throw std::runtime_error("Arguments must form a list");

        return func_name->function->apply(lex_env, arguments->toList());
    }
}

void Cons::emit_impl() const
{
    Emitter::emit(this->car);
    Emitter::emit(this->cdr);
}

std::string Cons::to_string_impl() const
{
    std::stringstream s;
    s << "(";
    s << Object::to_string(this->car);
    std::shared_ptr<Object> listIt = this->cdr;
    while (Object::is_true(listIt)) {
        s << " ";
        std::shared_ptr<Cons> maybeCons = std::dynamic_pointer_cast<Cons>(listIt);
        if (maybeCons) {
            s << Object::to_string(maybeCons->car);
            listIt = maybeCons->cdr;
        } else {
            s << ". ";
            s << Object::to_string(listIt);
            break;
        }
    }
    s << ")";

    return s.str();
}

bool Cons::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "cons" || sym->name == "list";
}

// --------------------------------------------------------------------------------

std::shared_ptr<Object> Nil::eval_impl(
    const std::shared_ptr<Object>& obj, Environment& lex_env [[maybe_unused]]) const
{
    return obj;
}

std::string Nil::to_string_impl() const
{
    return "nil";
}

bool Nil::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "null" || sym->name == "list";
}
