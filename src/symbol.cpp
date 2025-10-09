
#include "symbol.hpp"
#include "package.hpp"

void intern_symbols()
{
    std::shared_ptr<Symbol> t_sym = package.intern_symbol("t");
    t_sym->values = { t_sym };

    std::shared_ptr<Symbol> nil_sym = package.intern_symbol("nil");
    nil_sym->values = { std::make_shared<Nil>() };
}
