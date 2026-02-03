
#pragma once

#include "object.hpp"
#include <optional>

class Cons : public Object {
private:
    ObjectRef<Object> car;
    ObjectRef<Object> cdr;

public:
    Cons(Alma& alma, ObjectWeakRef<Object> _car, ObjectWeakRef<Object> _cdr);
    Cons(Alma& alma, const std::vector<ObjectWeakRef<Object>>& list);
    Cons(Alma& alma, const std::vector<ObjectWeakRef<Object>>& list, ObjectWeakRef<Object> non_proper_element);

    std::pair<std::vector<ObjectWeakRef<Object>>, ObjectWeakRef<Object>> to_list() const;

    virtual ObjectWeakRef<Object> eval(ObjectWeakRef<Object> self, ObjectWeakRef<Environment> enviroment)
        override;
    virtual std::string to_string(ObjectWeakRef<Object> self) override;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type) override;

    ObjectWeakRef<Object> get_car();
    ObjectWeakRef<Object> get_cdr();
};
