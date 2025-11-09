
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
