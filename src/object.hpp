
#pragma once

#include "garbage-collector.hpp"
#include <functional>
#include <string>

class Alma;
class Object;

template <typename T = Object>
class ObjectWeakRef;
template <typename T = Object>
class ObjectRef;

template <typename T, typename S>
concept Related = std::is_base_of_v<T, S> || std::is_base_of_v<S, T>;

// --------------------------------------------------------------------------------

class Object : protected GCObject {
public:
    virtual ObjectWeakRef<> eval(ObjectWeakRef<> self, Alma& alma);
    virtual std::string to_string(ObjectWeakRef<> self, Alma& alma);
    virtual bool typep(ObjectWeakRef<> self, ObjectWeakRef<> type, Alma& alma);
};

// --------------------------------------------------------------------------------

template <typename T>
class ObjectWeakRef {
    friend Alma;

protected:
    GCObject* obj;

public:
    template <Related<T> S>
    ObjectWeakRef(const ObjectWeakRef<S>& other);
    template <Related<T> S>
    ObjectWeakRef(ObjectWeakRef<S>&& other);
    template <Related<T> S>
    ObjectWeakRef(const ObjectRef<S>& other);
    template <Related<T> S>
    ObjectWeakRef(ObjectRef<S>&& other);
    ObjectWeakRef(std::nullptr_t) = delete;
    template <Related<T> S>
    ObjectWeakRef(S* obj);

    template <Related<T> S>
    ObjectWeakRef<T>& operator=(const ObjectWeakRef<S>& other);
    template <Related<T> S>
    ObjectWeakRef<T>& operator=(ObjectWeakRef<S>&& other);
    template <Related<T> S>
    ObjectWeakRef<T>& operator=(const ObjectRef<S>& other);
    template <Related<T> S>
    ObjectWeakRef<T>& operator=(ObjectRef<S>&& other);
    ObjectWeakRef<T>& operator=(std::nullptr_t) = delete;
    template <Related<T> S>
    ObjectWeakRef<T>& operator=(S* obj);

    template <Related<T> S>
    S* as();
    template <Related<T> S>
    const S* as() const;

    T* get();
    T& operator*();
    T* operator->();

    template <Related<T> S>
    bool operator==(const ObjectWeakRef<S>& other) const;
    template <Related<T> S>
    bool operator==(const ObjectRef<S>& other) const;
};

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>::ObjectWeakRef(const ObjectWeakRef<S>& other)
    : obj(other.obj)
{
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>::ObjectWeakRef(ObjectWeakRef<S>&& other)
    : obj(other.obj)
{
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>::ObjectWeakRef(const ObjectRef<S>& other)
    : obj(other.obj)
{
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>::ObjectWeakRef(ObjectRef<S>&& other)
    : obj(other.obj)
{
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>::ObjectWeakRef(S* _obj)
    : obj(_obj)
{
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(const ObjectWeakRef<S>& other)
{
    this->obj = other.obj;
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(ObjectWeakRef<S>&& other)
{
    this->obj = other.obj;
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(const ObjectRef<S>& other)
{
    this->obj = other.obj;
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(ObjectRef<S>&& other)
{
    this->obj = other.obj;
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(S* _obj)
{
    this->obj = _obj;
}

template <typename T>
template <Related<T> S>
S* ObjectWeakRef<T>::as()
{
    return this->obj;
}

template <typename T>
template <Related<T> S>
const S* ObjectWeakRef<T>::as() const
{
    return this->obj;
}

template <typename T>
T* ObjectWeakRef<T>::get()
{
    return this->obj;
}

template <typename T>
T& ObjectWeakRef<T>::operator*()
{
    return *this->obj;
}

template <typename T>
T* ObjectWeakRef<T>::operator->()
{
    return this->obj;
}

template <typename T>
template <Related<T> S>
bool ObjectWeakRef<T>::operator==(const ObjectWeakRef<S>& other) const
{
    return this->obj == other.obj;
}

template <typename T>
template <Related<T> S>
bool ObjectWeakRef<T>::operator==(const ObjectRef<S>& other) const
{
    return this->obj == other.obj;
}

// --------------------------------------------------------------------------------

template <typename T>
class ObjectRef : public ObjectWeakRef<T> {
private:
    Object& owner;

private:
    ObjectRef(Object& _owner)
        : owner(_owner)
    {
    }

public:
    template <Related<T> S>
    ObjectRef(Object& owner, const ObjectWeakRef<S>& other);
    template <Related<T> S>
    ObjectRef(Object& owner, ObjectWeakRef<S>&& other);
    template <Related<T> S>
    ObjectRef(Object& owner, const ObjectRef<S>& other);
    template <Related<T> S>
    ObjectRef(Object& owner, ObjectRef<S>&& other);
    ObjectRef(std::nullptr_t) = delete;
    template <Related<T> S>
    ObjectRef(Object& owner, S* obj);

    template <Related<T> S>
    ObjectRef<T>& operator=(const ObjectWeakRef<S>& other);
    template <Related<T> S>
    ObjectRef<T>& operator=(ObjectWeakRef<S>&& other);
    template <Related<T> S>
    ObjectRef<T>& operator=(const ObjectRef<S>& other);
    template <Related<T> S>
    ObjectRef<T>& operator=(ObjectRef<S>&& other);
    ObjectRef<T>& operator=(std::nullptr_t) = delete;
    template <Related<T> S>
    ObjectRef<T>& operator=(S* obj);
};

template <typename T>
template <Related<T> S>
ObjectRef<T>::ObjectRef(Object& owner, const ObjectWeakRef<S>& other)
    : ObjectWeakRef<T>(other)
    , ObjectRef(owner)
{
}

template <typename T>
template <Related<T> S>
ObjectRef<T>::ObjectRef(Object& owner, ObjectWeakRef<S>&& other)
    : ObjectWeakRef<T>(other)
    , ObjectRef(owner)
{
}

template <typename T>
template <Related<T> S>
ObjectRef<T>::ObjectRef(Object& owner, const ObjectRef<S>& other)
    : ObjectWeakRef<T>(other)
    , ObjectRef(owner)
{
}

template <typename T>
template <Related<T> S>
ObjectRef<T>::ObjectRef(Object& owner, ObjectRef<S>&& other)
    : ObjectWeakRef<T>(other)
    , ObjectRef(owner)
{
}

template <typename T>
template <Related<T> S>
ObjectRef<T>::ObjectRef(Object& owner, S* _obj)
    : ObjectWeakRef<T>(_obj)
    , ObjectRef(owner)
{
}

template <typename T>
template <Related<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(const ObjectWeakRef<S>& other)
{
    this->obj = other.obj;
}

template <typename T>
template <Related<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(ObjectWeakRef<S>&& other)
{
    this->obj = other.obj;
}

template <typename T>
template <Related<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(const ObjectRef<S>& other)
{
    this->obj = other.obj;
}

template <typename T>
template <Related<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(ObjectRef<S>&& other)
{
    this->obj = other.obj;
}

template <typename T>
template <Related<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(S* _obj)
{
    this->obj = _obj;
}

// --------------------------------------------------------------------------------

template <typename T>
struct std::hash<ObjectWeakRef<T>> {
    std::size_t operator()(const ObjectWeakRef<T>& obj)
    {
        return std::hash<GCObject*>()(obj.obj);
    }
};

template <typename T>
struct std::hash<ObjectRef<T>> {
    std::size_t operator()(const ObjectRef<T>& obj)
    {
        return std::hash<GCObject*>()(obj.obj);
    }
};
