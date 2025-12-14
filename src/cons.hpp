
#pragma once

#include "object.hpp"
#include <optional>

class Cons : public Object {
private:
    ObjectRef<Object> car;
    ObjectRef<Object> cdr;

public:
    class iterator {
    private:
        Alma& alma;
        std::optional<ObjectWeakRef<Cons>> ref; // cons or nil

    private:
        void increment();

    public:
        iterator(Alma& alma);
        iterator(ObjectWeakRef<Cons> ref, Alma& alma);
        iterator(const iterator& other);

        iterator& operator=(const iterator& other);

        iterator& operator++();
        iterator operator++(int);

        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;

        Object& operator*();
        Object* operator->();

        template <typename T>
            requires std::is_base_of_v<T, Cons>
        operator ObjectWeakRef<T>();
    };

public:
    Cons(ObjectWeakRef<Object> _car, ObjectWeakRef<Object> _cdr);
    // Cons(const std::vector<ObjectWeakRef<Object>>& list);

    virtual ObjectWeakRef<Object> eval(ObjectWeakRef<Object> self, Alma& alma) const override;
    virtual std::string to_string(ObjectWeakRef<Object> self, Alma& alma) const override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const override;

    iterator begin(Alma& alma);
    iterator end(Alma& alma);
};

template <typename T>
    requires std::is_base_of_v<T, Cons>
Cons::iterator::operator ObjectWeakRef<T>()
{
    return this->ref;
}

#define alma_for_each(VAR, LIST, ALMA) \
    for (auto VAR = LIST->begin(ALMA); VAR != LIST->end(ALMA); ++VAR)
