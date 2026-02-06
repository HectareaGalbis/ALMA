
#include "package.hpp"
#include "alma.hpp"
#include "debug.hpp"
#include "symbol.hpp"

Package::Package(Alma& _alma)
    : Object(_alma)
{
}

ObjectRef<Object> Package::find_symbol(const std::string& name)
{
    if (this->symbols.contains(name)) {
        return this->symbols.at(name);
    } else {
        return this->alma.intern_alma_symbol("nil");
    }
}

ObjectRef<Symbol> Package::intern_symbol(const std::string& name)
{
    if (!this->symbols.contains(name))
        this->symbols.try_emplace(name, *this, this->alma.make<Symbol>(name));
    return this->symbols.at(name);
}

bool Package::typep(ObjectRef<Object> self [[maybe_unused]], ObjectRef<Object> type)
{
    return type == this->alma.intern_alma_symbol("package") || this->Object::typep(self, type);
}
