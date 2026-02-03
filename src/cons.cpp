
#include "cons.hpp"
#include "alma.hpp"
#include "debug.hpp"
#include "package.hpp"
#include "procedure.hpp"
#include "symbol.hpp"

// -----------------------------------------------------------------------------

Cons::Cons(Alma& _alma, ObjectWeakRef<Object> _car, ObjectWeakRef<Object> _cdr)
    : Object(_alma)
    , car(*this, _car)
    , cdr(*this, _cdr)
{
}

static ObjectWeakRef<Object> get_list_car(const std::vector<ObjectWeakRef<Object>>& list)
{
    if (list.empty())
        mthrow("The list must not be empty");

    return list.front();
}

static ObjectWeakRef<Object> get_list_cdr(const std::vector<ObjectWeakRef<Object>>& list,
    size_t currentIndex, ObjectWeakRef<Object> non_proper_element, Alma& alma)
{
    if (list.empty())
        mthrow("The list must not be empty");

    if (currentIndex >= list.size())
        return non_proper_element;
    else
        return alma.make<Cons>(
            list[currentIndex], get_list_cdr(list, currentIndex + 1, non_proper_element, alma));
}

Cons::Cons(Alma& _alma, const std::vector<ObjectWeakRef<Object>>& list)
    : Object(_alma)
    , car(*this, get_list_car(list))
    , cdr(*this, get_list_cdr(list, 1, alma.find_alma_symbol("nil"), alma))
{
}

Cons::Cons(Alma& _alma, const std::vector<ObjectWeakRef<Object>>& list, ObjectWeakRef<Object> non_proper_element)
    : Object(_alma)
    , car(*this, get_list_car(list))
    , cdr(*this, get_list_cdr(list, 1, non_proper_element, alma))
{
}

std::pair<std::vector<ObjectWeakRef<Object>>, ObjectWeakRef<Object>> Cons::to_list() const
{
    std::vector<ObjectWeakRef<Object>> list;
    list.push_back(this->car);
    ObjectWeakRef<Object> argIt = this->cdr;
    while (argIt) {
        if (!this->alma.consp(argIt)) {
            return { list, argIt };
        }
        list.push_back(argIt.as<Cons>()->car);
        argIt = argIt.as<Cons>()->cdr;
    }
    return { list, this->alma.find_alma_symbol("nil") };
}

ObjectWeakRef<Object> Cons::eval(
    ObjectWeakRef<Object> self [[maybe_unused]],
    ObjectWeakRef<Environment> environment)
{
    return this->alma.apply(this->alma.eval(this->car, environment), this->cdr, environment);
}

std::string Cons::to_string(ObjectWeakRef<Object> self [[maybe_unused]])
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

bool Cons::typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type)
{
    return type == this->alma.find_alma_symbol("cons") || this->Object::typep(self, type);
}

ObjectWeakRef<Object> Cons::get_car()
{
    return this->car;
}

ObjectWeakRef<Object> Cons::get_cdr()
{
    return this->cdr;
}
