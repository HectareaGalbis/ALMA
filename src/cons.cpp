
#include "cons.hpp"

Cons::Cons(const std::shared_ptr<Object>& _car, const std::shared_ptr<Object>& _cdr)
    : car(_car)
    , cdr(_cdr)
{
}

static std::shared_ptr<Cons> makeConsFromList(const std::vector<std::shared_ptr<Object>>& list,
    size_t currentIndex)
{
    if (currentIndex == list.size() - 1) {
        return std::make_shared<Cons>(list[currentIndex], std::make_shared<Nil>());
    } else {
        return std::make_shared<Cons>(list[currentIndex], makeConsFromList(list, currentIndex + 1));
    }
}

Cons::Cons(const std::vector<std::shared_ptr<Object>>& list)
{
    if (list.empty())
        throw std::runtime_error("The list is empty");

    std::shared_ptr<Cons> newCons = makeConsFromList(list, 0);
    this->car = newCons->car;
    this->cdr = newCons->cdr;
}

std::vector<std::shared_ptr<Object>> Cons::toList() const
{
    std::vector<std::shared_ptr<Object>> list;
    list.push_back(this->car);
    std::shared_ptr<Object> argIt = this->cdr;
    while (Object::is_true(argIt)) {
        std::shared_ptr<Cons> consIt = std::dynamic_pointer_cast<Cons>(argIt);
        if (!consIt)
            throw std::runtime_error("Error: Not a proper list.");
        list.push_back(consIt->car);
        argIt = consIt->cdr;
    }
    return list;
}

std::shared_ptr<Object> Cons::eval_impl(
    const std::shared_ptr<Object>& obj [[maybe_unused]], Environment& lex_env) const
{
    std::shared_ptr<Symbol> func_name = std::dynamic_pointer_cast<Symbol>(this->car);
    if (!func_name)
        throw std::runtime_error("Expected a symbol denoting a procedure. Found a " + Object::to_string(this->car));
    if (!func_name->function)
        throw std::runtime_error("The symbol " + func_name->name + " does not denote a procedure.");

    if (!Object::is_true(this->cdr)) {
        return func_name->function->apply(lex_env, {});
    } else {
        std::shared_ptr<Cons> arguments = std::dynamic_pointer_cast<Cons>(this->cdr);
        if (!arguments)
            throw std::runtime_error("Arguments must form a list");

        return func_name->function->apply(lex_env, arguments->toList());
    }
}

void Cons::emit_impl() const
{
    Emitter::emit(this->car);
    Emitter::emit(this->cdr);
}

std::string Cons::to_string_impl() const
{
    std::stringstream s;
    s << "(";
    s << Object::to_string(this->car);
    std::shared_ptr<Object> listIt = this->cdr;
    while (Object::is_true(listIt)) {
        s << " ";
        std::shared_ptr<Cons> maybeCons = std::dynamic_pointer_cast<Cons>(listIt);
        if (maybeCons) {
            s << Object::to_string(maybeCons->car);
            listIt = maybeCons->cdr;
        } else {
            s << ". ";
            s << Object::to_string(listIt);
            break;
        }
    }
    s << ")";

    return s.str();
}

bool Cons::typep_impl(const std::shared_ptr<Symbol>& sym) const
{
    return sym->name == "cons" || sym->name == "list";
}
