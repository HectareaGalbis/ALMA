
#include "package.hpp"
#include "alma.hpp"
#include "symbol.hpp"

Package::Package(Alma& _alma)
    : Object(_alma)
{
}

ObjectWeakRef<Object> Package::find_symbol(const std::string& name)
{
    if (this->symbols.contains(name)) {
        return this->symbols.at(name);
    } else {
        return this->alma.find_alma_symbol("nil");
    }
}

ObjectWeakRef<Symbol> Package::intern_symbol(const std::string& name)
{
    if (!this->symbols.contains(name))
        this->symbols.try_emplace(name, *this, this->alma.make<Symbol>(name));
    return this->symbols.at(name);
}

bool Package::typep(ObjectWeakRef<Object> self [[maybe_unused]], ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("package") || this->Object::typep(self, type);
}
