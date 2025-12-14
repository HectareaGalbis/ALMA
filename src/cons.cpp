
#include "cons.hpp"
#include "alma.hpp"
#include "debug.hpp"
#include "package.hpp"
#include "procedure.hpp"
#include "symbol.hpp"

void Cons::iterator::increment()
{
    massert(this->ref, "The iterator has ended");

    ObjectWeakRef<Cons> cons_ref = *this->ref;

    if (alma.consp(cons_ref->cdr)) {
        this->ref = cons_ref->cdr.as<Cons>();
    } else if (alma.null(cons_ref->cdr)) {
        this->ref = std::nullopt;
    } else {
        mthrow("The cons is not a proper list");
    }
}

Cons::iterator::iterator(Alma& _alma)
    : alma(_alma)
{
}

Cons::iterator::iterator(ObjectWeakRef<Cons> _ref, Alma& _alma)
    : alma(_alma)
    , ref(_ref)
{
}

Cons::iterator::iterator(const iterator& other)
    : alma(other.alma)
    , ref(other.ref)
{
}

Cons::iterator& Cons::iterator::operator=(const iterator& other)
{
    this->ref = other.ref;
}

Cons::iterator& Cons::iterator::operator++()
{
    this->increment();
    return *this;
}

Cons::iterator Cons::iterator::operator++(int)
{
    iterator old(*this);
    this->increment();
    return old;
}

bool Cons::iterator::operator==(const iterator& other) const
{
    return this->ref == other.ref;
}

bool Cons::iterator::operator!=(const iterator& other) const
{
    return this->ref != other.ref;
}

Object& Cons::iterator::operator*()
{
    massert(this->ref, "Cannot dereference. The iterator has ended");
    return *(*this->ref)->car;
}

Object* Cons::iterator::operator->()
{
    massert(this->ref, "Cannot dereference. The iterator has ended");
    return (*this->ref)->car.get();
}

// --------------------------------------------------------------------------------

Cons::Cons(ObjectWeakRef<Object> _car, ObjectWeakRef<Object> _cdr)
    : car(*this, _car)
    , cdr(*this, _cdr)
{
}

// static std::shared_ptr<Cons> makeConsFromList(const std::vector<std::shared_ptr<Object>>& list,
//     size_t currentIndex)
// {
//     if (currentIndex == list.size() - 1) {
//         return std::make_shared<Cons>(list[currentIndex], std::make_shared<Nil>());
//     } else {
//         return std::make_shared<Cons>(list[currentIndex], makeConsFromList(list, currentIndex + 1));
//     }
// }

// Cons::Cons(const std::vector<std::shared_ptr<Object>>& list)
// {
//     if (list.empty())
//         throw std::runtime_error("The list is empty");

//     std::shared_ptr<Cons> newCons = makeConsFromList(list, 0);
//     this->car = newCons->car;
//     this->cdr = newCons->cdr;
// }

// std::vector<std::shared_ptr<Object>> Cons::toList() const
// {
//     std::vector<std::shared_ptr<Object>> list;
//     list.push_back(this->car);
//     std::shared_ptr<Object> argIt = this->cdr;
//     while (Object::is_true(argIt)) {
//         std::shared_ptr<Cons> consIt = std::dynamic_pointer_cast<Cons>(argIt);
//         if (!consIt)
//             throw std::runtime_error("Error: Not a proper list.");
//         list.push_back(consIt->car);
//         argIt = consIt->cdr;
//     }
//     return list;
// }

ObjectWeakRef<Object> Cons::eval(ObjectWeakRef<Object> self, Alma& alma) const
{
    massert(alma.symbolp(this->car), "Expected a symbol denoting a procedure. Found a " << alma.to_string(this->car));

    ObjectWeakRef<Symbol> sym = this->car.as<Symbol>();
    massert(sym->get_function(), "The symbol " << sym->get_name() << " does not denote a procedure.");

    sym->get_function()->apply(this->cdr, alma);
}

std::string Cons::to_string(ObjectWeakRef<Object> self, Alma& alma) const
{
    std::stringstream s;
    s << "(";
    s << alma.to_string(this->car);
    ObjectWeakRef<Object> it = this->cdr;
    while (alma.truep(it)) {
        s << " ";
        if (alma.consp(it)) {
            s << alma.to_string(it.as<Cons>()->car);
        } else {
            s << ". ";
            s << alma.to_string(it);
            break;
        }
        it = it.as<Cons>()->cdr;
    }
    s << ")";

    return s.str();
}

bool Cons::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const
{
    return type == alma.find_alma_symbol("cons") || this->Object::typep(self, type, alma);
}

Cons::iterator Cons::begin(Alma& alma)
{
    return iterator(this, alma);
}

Cons::iterator Cons::end(Alma& alma)
{
    return iterator(alma);
}
