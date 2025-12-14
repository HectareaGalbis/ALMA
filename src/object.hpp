
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

class Object : public GCObject {
public:
    virtual ObjectWeakRef<Object> eval(ObjectWeakRef<Object> self, Alma& alma) const;
    virtual std::string to_string(ObjectWeakRef<Object> self, Alma& alma) const;
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type, Alma& alma) const;
    bool truep(ObjectWeakRef<Object> self, Alma& alma) const;
};

// --------------------------------------------------------------------------------

template <typename T>
class ObjectWeakRef {
    friend Alma;
    template <typename S>
    friend class ObjectRef;
    template <typename S>
    friend class ObjectWeakRef;
    friend struct ObjectRefHash;
    friend struct ObjectRefEqual;

public:
    template <typename S>
    using rebind = ObjectWeakRef<S>;
    using value_type = T;

private:
    GCObject* obj;

public:
    template <std::derived_from<T> S>
    ObjectWeakRef(const ObjectWeakRef<S>& other);
    template <std::derived_from<T> S>
    ObjectWeakRef(ObjectWeakRef<S>&& other);
    template <std::derived_from<T> S>
    ObjectWeakRef(const ObjectRef<S>& other);
    template <std::derived_from<T> S>
    ObjectWeakRef(ObjectRef<S>&& other);
    ObjectWeakRef(std::nullptr_t) = delete;
    template <std::derived_from<T> S>
    ObjectWeakRef(S* obj);

    template <std::derived_from<T> S>
    ObjectWeakRef<T>& operator=(const ObjectWeakRef<S>& other);
    template <std::derived_from<T> S>
    ObjectWeakRef<T>& operator=(ObjectWeakRef<S>&& other);
    template <std::derived_from<T> S>
    ObjectWeakRef<T>& operator=(const ObjectRef<S>& other);
    template <std::derived_from<T> S>
    ObjectWeakRef<T>& operator=(ObjectRef<S>&& other);
    ObjectWeakRef<T>& operator=(std::nullptr_t) = delete;
    template <std::derived_from<T> S>
    ObjectWeakRef<T>& operator=(S* obj);

    template <std::derived_from<T> S>
    S* as();
    template <std::derived_from<T> S>
    const S* as() const;

    T* get();
    T& operator*();
    T* operator->();
    const T* get() const;
    const T& operator*() const;
    const T* operator->() const;

    template <Related<T> S>
    bool operator==(const ObjectWeakRef<S>& other) const;
    template <Related<T> S>
    bool operator==(const ObjectRef<S>& other) const;
};

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>::ObjectWeakRef(const ObjectWeakRef<S>& other)
    : obj(other.obj)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>::ObjectWeakRef(ObjectWeakRef<S>&& other)
    : obj(other.obj)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>::ObjectWeakRef(const ObjectRef<S>& other)
    : obj(static_cast<T*>(other.obj))
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>::ObjectWeakRef(ObjectRef<S>&& other)
    : obj(other.obj)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>::ObjectWeakRef(S* _obj)
    : obj(_obj)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(const ObjectWeakRef<S>& other)
{
    this->obj = other.obj;
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(ObjectWeakRef<S>&& other)
{
    this->obj = other.obj;
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(const ObjectRef<S>& other)
{
    this->obj = other.obj;
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(ObjectRef<S>&& other)
{
    this->obj = other.obj;
}

template <typename T>
template <std::derived_from<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(S* _obj)
{
    this->obj = _obj;
}

template <typename T>
template <std::derived_from<T> S>
S* ObjectWeakRef<T>::as()
{
    return this->obj;
}

template <typename T>
template <std::derived_from<T> S>
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
    return static_cast<T*>(this->obj);
}

template <typename T>
const T* ObjectWeakRef<T>::get() const
{
    return static_cast<T*>(this->obj);
}

template <typename T>
const T& ObjectWeakRef<T>::operator*() const
{
    return *this->obj;
}

template <typename T>
const T* ObjectWeakRef<T>::operator->() const
{
    return static_cast<T*>(this->obj);
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
    template <typename S>
    friend class ObjectWeakRef;
    template <typename S>
    friend class ObjectRef;

public:
    template <typename S>
    using rebind = ObjectRef<S>;

private:
    Object& owner;

public:
    template <std::derived_from<T> S>
    ObjectRef(Object& owner, const ObjectWeakRef<S>& other);
    template <std::derived_from<T> S>
    ObjectRef(Object& owner, ObjectWeakRef<S>&& other);
    template <std::derived_from<T> S>
    ObjectRef(Object& owner, const ObjectRef<S>& other);
    template <std::derived_from<T> S>
    ObjectRef(Object& owner, ObjectRef<S>&& other);
    ObjectRef(std::nullptr_t) = delete;
    template <std::derived_from<T> S>
    ObjectRef(Object& owner, S* obj);
    // template <Related<T> S>
    // ObjectRef(ObjectRef<S>&& other);
    // template <Related<T> S>
    // ObjectRef(ObjectWeakRef<S>&& other);

    template <std::derived_from<T> S>
    ObjectRef<T>& operator=(const ObjectWeakRef<S>& other);
    template <std::derived_from<T> S>
    ObjectRef<T>& operator=(ObjectWeakRef<S>&& other);
    template <std::derived_from<T> S>
    ObjectRef<T>& operator=(const ObjectRef<S>& other);
    template <std::derived_from<T> S>
    ObjectRef<T>& operator=(ObjectRef<S>&& other);
    ObjectRef<T>& operator=(std::nullptr_t) = delete;
    template <std::derived_from<T> S>
    ObjectRef<T>& operator=(S* obj);
};

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>::ObjectRef(Object& _owner, const ObjectWeakRef<S>& other)
    : ObjectWeakRef<T>(other)
    , owner(_owner)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>::ObjectRef(Object& _owner, ObjectWeakRef<S>&& other)
    : ObjectWeakRef<T>(other)
    , owner(_owner)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>::ObjectRef(Object& _owner, const ObjectRef<S>& other)
    : ObjectWeakRef<T>(other)
    , owner(_owner)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>::ObjectRef(Object& _owner, ObjectRef<S>&& other)
    : ObjectWeakRef<T>(other)
    , owner(_owner)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>::ObjectRef(Object& _owner, S* _obj)
    : ObjectWeakRef<T>(_obj)
    , owner(_owner)
{
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(const ObjectWeakRef<S>& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(ObjectWeakRef<S>&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(const ObjectRef<S>& other)
{
    this->obj = other.obj;
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(ObjectRef<S>&& other)
{
    this->obj = other.obj;
}

template <typename T>
template <std::derived_from<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(S* _obj)
{
    this->obj = _obj;
}

// --------------------------------------------------------------------------------

template <typename T, typename S>
concept ObjectRefType
    = (std::is_same_v<typename T::template rebind<int>, ObjectWeakRef<int>>
          || std::is_same_v<typename T::template rebind<int>, ObjectRef<int>>)
    && std::is_base_of_v<S, typename T::value_type>;

// --------------------------------------------------------------------------------

// Make ObjectRef and ObjectWeakRef hashable and transparent to be usable in std::unordered_map

struct ObjectRefHash {
    using is_transparent = void;

    template <typename T>
    std::size_t operator()(const ObjectWeakRef<T>& obj) const noexcept
    {
        return std::hash<GCObject*>()(obj.obj);
    }

    template <typename T>
    std::size_t operator()(const ObjectRef<T>& obj) const noexcept
    {
        return std::hash<GCObject*>()(obj.obj);
    }
};

struct ObjectRefEqual {
    using is_transparent = void;

    template <typename T, typename S>
    bool operator()(const ObjectWeakRef<T>& obj1, const ObjectWeakRef<S>& obj2) const noexcept
    {
        return obj1.obj == obj2.obj;
    }

    template <typename T, typename S>
    bool operator()(const ObjectWeakRef<T>& obj1, const ObjectRef<S>& obj2) const noexcept
    {
        return obj1.obj == obj2.obj;
    }

    template <typename T, typename S>
    bool operator()(const ObjectRef<T>& obj1, const ObjectWeakRef<S>& obj2) const noexcept
    {
        return obj1.obj == obj2.obj;
    }

    template <typename T, typename S>
    bool operator()(const ObjectRef<T>& obj1, const ObjectRef<S>& obj2) const noexcept
    {
        return obj1.obj == obj2.obj;
    }
};
