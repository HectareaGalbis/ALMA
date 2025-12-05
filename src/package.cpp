
#include "package.hpp"
#include "alma.hpp"
#include "symbol.hpp"

ObjectWeakRef<Object> Package::find_symbol(const std::string& name, Alma& alma)
{
    if (this->symbols.contains(name)) {
        return this->symbols.at(name);
    } else {
        return alma.alma_package->find_symbol("nil", alma);
    }
}

ObjectWeakRef<Object> Package::intern_symbol(const std::string& name, Alma& alma)
{
    if (!this->symbols.contains(name))
        this->symbols.try_emplace(name, ObjectRef(*this, alma.gc.make_object<Symbol>(name)));
    return this->symbols[name];
}

bool Package::typep(ObjectWeakRef<Object> self [[maybe_unused]], ObjectWeakRef<Object> type, Alma& alma)
{
    return type == alma.alma_package->find_symbol("package", alma);
}
