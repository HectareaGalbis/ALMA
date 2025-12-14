
#include "symbol.hpp"
#include "objects.hpp"
#include "package.hpp"

void intern_symbols()
{
    Package::almaPackage->intern_symbol("unquote");
    Package::almaPackage->intern_symbol("slice-unquote");

    std::shared_ptr<Symbol> current_package_sym = Package::almaPackage->intern_symbol("*current-package*");
    current_package_sym->values = { Package::currentPackage };

    std::shared_ptr<Symbol> t_sym = Package::almaPackage->intern_symbol("t");
    t_sym->values = { t_sym };

    std::shared_ptr<Symbol> nil_sym = Package::almaPackage->intern_symbol("nil");
    nil_sym->values = { std::make_shared<Nil>() };
}

Symbol::Symbol(const std::string& _name)
    : name(_name)
{
}

ObjectWeakRef<Object> Symbol::eval(ObjectWeakRef<Object> self, Alma& alma)
{
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

std::string& Symbol::get_name()
{
    return this->name;
}

const std::string& Symbol::get_name() const
{
    return this->name;
}
