
#include "symbol.hpp"
#include "alma.hpp"
#include "debug.hpp"
#include "environment.hpp"
#include "object.hpp"
#include "package.hpp"

// void intern_symbols()
// {
//     Package::almaPackage->intern_symbol("unquote");
//     Package::almaPackage->intern_symbol("slice-unquote");

//     std::shared_ptr<Symbol> current_package_sym = Package::almaPackage->intern_symbol("*current-package*");
//     current_package_sym->values = { Package::currentPackage };

//     std::shared_ptr<Symbol> t_sym = Package::almaPackage->intern_symbol("t");
//     t_sym->values = { t_sym };

//     std::shared_ptr<Symbol> nil_sym = Package::almaPackage->intern_symbol("nil");
//     nil_sym->values = { std::make_shared<Nil>() };
// }

Symbol::Symbol(Alma& _alma, const std::string& _name)
    : Object(_alma)
    , name(_name)
{
}

ObjectWeakRef<Object> Symbol::eval(ObjectWeakRef<Object> self, ObjectWeakRef<Environment> environment)
{
    std::optional<ObjectWeakRef<Object>> lex_value
        = environment->get_value(self, this->alma.find_alma_symbol("value"));
    if (lex_value) {
        return *lex_value;
    } else {
        return this->get_value();
    }
}

std::string Symbol::to_string(ObjectWeakRef<Object> self [[maybe_unused]])
{
    return this->name;
}

bool Symbol::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("symbol") || this->Object::typep(self, type);
}

std::string& Symbol::get_name()
{
    return this->name;
}

const std::string& Symbol::get_name() const
{
    return this->name;
}

bool Symbol::has_property(ObjectWeakRef<Symbol> property)
{
    return (this->properties.contains(property) && this->properties.find(property)->second.size() > 0);
}

ObjectWeakRef<Object> Symbol::get_property(ObjectWeakRef<Symbol> property)
{
    auto it = this->properties.find(property);
    if (it != this->properties.end() && it->second.size() > 0) {
        return it->second.back();
    }
    mthrow("No property " << property->get_name() << " found for the symbol " << this->name);
}

void Symbol::set_property(ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    auto it = this->properties.find(property);
    if (it != this->properties.end() && it->second.size() > 0) {
        it->second.back() = value;
    }
    mthrow("No property " << property->get_name() << " found for the symbol " << this->name);
}

void Symbol::push_property(ObjectWeakRef<Symbol> property, ObjectWeakRef<Object> value)
{
    auto it = this->properties.find(property);
    if (it == this->properties.end())
        this->properties.try_emplace(ObjectTrackedRef<Symbol>(*this, property));
    this->properties.find(property)->second.emplace_back(*this, value);
}

void Symbol::pop_property(ObjectWeakRef<Symbol> property)
{
    auto it = this->properties.find(property);
    if (it != this->properties.end() && it->second.size() > 0) {
        it->second.pop_back();
    }
    mthrow("No property " << property->get_name() << " found for the symbol " << this->name);
}

bool Symbol::has_value()
{
    return this->has_property(this->alma.find_alma_symbol("value"));
}

ObjectWeakRef<Object> Symbol::get_value()
{
    return this->get_property(this->alma.find_alma_symbol("value"));
}

void Symbol::set_value(ObjectWeakRef<Object> value)
{
    this->set_property(this->alma.find_alma_symbol("value"), value);
}

void Symbol::push_value(ObjectWeakRef<Object> value)
{
    this->push_property(this->alma.find_alma_symbol("value"), value);
}

void Symbol::pop_value()
{
    this->pop_property(this->alma.find_alma_symbol("value"));
}

bool Symbol::has_package()
{
    return this->has_property(this->alma.find_alma_symbol("package"));
}

ObjectWeakRef<Package> Symbol::get_package()
{
    return this->get_property(this->alma.find_alma_symbol("package"));
}

void Symbol::set_package(ObjectWeakRef<Package> package)
{
    this->set_property(this->alma.find_alma_symbol("package"), package);
}

void Symbol::push_package(ObjectWeakRef<Package> package)
{
    this->push_property(this->alma.find_alma_symbol("package"), package);
}

void Symbol::pop_package()
{
    this->pop_property(this->alma.find_alma_symbol("package"));
}
