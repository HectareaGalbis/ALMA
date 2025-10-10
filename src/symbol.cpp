
#include "symbol.hpp"
#include "objects.hpp"
#include "package.hpp"

void intern_symbols()
{
    std::shared_ptr<Symbol> current_package_sym = Package::almaPackage->intern_symbol("*current-package*");
    current_package_sym->values = { Package::currentPackage };

    std::shared_ptr<Symbol> casa_sym = Package::almaPackage->intern_symbol("casa");
    casa_sym->package = std::make_shared<Package>();
    std::shared_ptr<Symbol> pera = casa_sym->package->intern_symbol("pera");
    pera->values = { std::make_shared<Integer>(5) };

    std::shared_ptr<Symbol> t_sym = Package::almaPackage->intern_symbol("t");
    t_sym->values = { t_sym };

    std::shared_ptr<Symbol> nil_sym = Package::almaPackage->intern_symbol("nil");
    nil_sym->values = { std::make_shared<Nil>() };
}
