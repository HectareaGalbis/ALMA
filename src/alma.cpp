

#include "alma.hpp"
#include "cons.hpp"
#include "debug.hpp"
#include "environment.hpp"
#include "function.hpp"
#include "object.hpp"
#include "package.hpp"
#include "reader.hpp"
#include <fstream>

#define intern_function(name, sym_name)                                     \
    ObjectWeakRef<Symbol> name##_func = this->intern_alma_symbol(sym_name); \
    name##_func->push_value(this->make<name>());

void Alma::intern_functions()
{
    intern_function(Sum, "+");
    intern_function(Print, "print");
    intern_function(Typep, "typep");
    intern_function(Set_symbol_value, "set-symbol-value");
    intern_function(Car, "car");
    intern_function(Cdr, "cdr");
    intern_function(Setq, "setq");
    intern_function(Eq, "eq");
    intern_function(Eval, "eval");
}

void Alma::intern_symbols()
{
}

Alma::Alma()
    : gc()
    , environment(*this, gc.make_object<Environment>(*this))
    , alma_package(*this, gc.make_object<Package>(*this))
    , current_package(alma_package)
{
    this->intern_functions();
    this->intern_symbols();
}

ObjectWeakRef<Object> Alma::eval(ObjectWeakRef<Object> obj, ObjectWeakRef<Environment> lex_environment)
{
    return obj->eval(obj, lex_environment);
}

ObjectWeakRef<Object> Alma::eval(ObjectWeakRef<Object> obj)
{
    return this->eval(obj, this->environment);
}

void Alma::load(const std::filesystem::path& path)
{
    std::ifstream input(path);
    Reader reader(*this, path.native(), input);
    for (std::optional<ObjectWeakRef<Object>> obj = reader.read(); obj; obj = reader.read()) {
        this->eval(*obj);
    }
}

ObjectWeakRef<Object> Alma::apply(
    ObjectWeakRef<Object> obj,
    const std::vector<ObjectWeakRef<Object>>& arg_list,
    ObjectWeakRef<Environment> _environment)
{
    return obj->apply(obj, arg_list, _environment);
}

ObjectWeakRef<Object> Alma::apply(
    ObjectWeakRef<Object> obj,
    ObjectWeakRef<Cons> args,
    ObjectWeakRef<Environment> _environment)
{
    auto [arg_list, arg_rest] = args->to_list();
    massert(arg_rest, "A non proper list cannot be applied");
    return this->apply(obj, arg_list, _environment);
}

ObjectWeakRef<Package> Alma::get_current_package()
{
    return this->current_package;
}

std::string Alma::to_string(ObjectWeakRef<Object> obj)
{
    return obj->to_string(obj);
}

bool Alma::typep(ObjectWeakRef<Object> obj, ObjectWeakRef<Object> sym)
{
    return obj->typep(obj, sym);
}

bool Alma::alma_typep(ObjectWeakRef<Object> obj, const std::string& type)
{
    return this->typep(obj, this->find_alma_symbol(type));
}

bool Alma::symbolp(ObjectWeakRef<Object> obj)
{
    return this->typep(obj, this->find_alma_symbol("symbol"));
}

ObjectWeakRef<Object> Alma::symbol_value(ObjectWeakRef<Symbol> symbol)
{
    return symbol->get_value();
}

ObjectWeakRef<Object> Alma::set_symbol_value(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value)
{
    symbol->set_value(value);
    return value;
}

bool Alma::consp(ObjectWeakRef<Object> obj)
{
    return this->typep(obj, this->find_alma_symbol("cons"));
}

bool Alma::truep(ObjectWeakRef<Object> obj)
{
    return obj != this->find_alma_symbol("nil");
}

bool Alma::null(ObjectWeakRef<Object> obj)
{
    return obj == this->find_alma_symbol("nil");
}

ObjectWeakRef<Object> Alma::boolean(bool v)
{
    return this->find_alma_symbol(v ? "t" : "nil");
}

bool Alma::eq(ObjectWeakRef<Object> obj1, ObjectWeakRef<Object> obj2)
{
    return obj1 == obj2;
}

ObjectWeakRef<Object> Alma::setq(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value,
    ObjectWeakRef<Environment> _environment)
{
    ObjectWeakRef<Symbol> sym_value = this->find_alma_symbol("value");
    if (_environment->has_symbol_property(symbol, sym_value)) {
        _environment->set_value(symbol, sym_value, value);
    } else {
        symbol->set_value(value);
    }
    return value;
}

ObjectWeakRef<Object> Alma::setq(ObjectWeakRef<Symbol> symbol, ObjectWeakRef<Object> value)
{
    return this->setq(symbol, value, this->environment);
}

ObjectWeakRef<Object> Alma::find_symbol(const std::string& name, ObjectWeakRef<Package> package)
{
    return package->find_symbol(name);
}

ObjectWeakRef<Object> Alma::find_symbol(const std::string& name)
{
    return this->current_package->find_symbol(name);
}

ObjectWeakRef<Object> Alma::find_alma_symbol(const std::string& name)
{
    return this->alma_package->find_symbol(name);
}

ObjectWeakRef<Object> Alma::intern_symbol(const std::string& name, ObjectWeakRef<Package> package)
{
    return package->intern_symbol(name);
}

ObjectWeakRef<Object> Alma::intern_symbol(const std::string& name)
{
    return this->current_package->intern_symbol(name);
}

ObjectWeakRef<Object> Alma::intern_alma_symbol(const std::string& name)
{
    return this->alma_package->intern_symbol(name);
}

ObjectWeakRef<Object> Alma::car(ObjectWeakRef<Cons> c)
{
    return c->get_car();
}

ObjectWeakRef<Object> Alma::cdr(ObjectWeakRef<Cons> c)
{
    return c->get_cdr();
}
