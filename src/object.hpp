
#pragma once

#include "garbage-collector.hpp"
#include <functional>
#include <optional>
#include <string>

class Alma;
class Object;
class Environment;
class Cons;

template <typename T>
class ObjectWeakRef;
template <typename T>
class ObjectRef;
template <typename T>
class ObjectProtectedRef;

// -----------------------------------------------------------------------------

template <typename T, typename S>
concept Related = std::is_base_of_v<T, S> || std::is_base_of_v<S, T>;

// -----------------------------------------------------------------------------

template <typename T, typename S>
concept ObjectRefType
    = (std::is_same_v<typename std::remove_cvref_t<T>::template rebind<int>, ObjectWeakRef<int>>
          || std::is_same_v<typename std::remove_cvref_t<T>::template rebind<int>, ObjectRef<int>>
          || std::is_same_v<typename std::remove_cvref_t<T>::template rebind<int>, ObjectProtectedRef<int>>)
    && (std::is_base_of_v<S, typename std::remove_cvref_t<T>::value_type>
        || std::is_base_of_v<typename std::remove_cvref_t<T>::value_type, S>);

// -----------------------------------------------------------------------------

class Object : public GCObject {
    template <typename S>
    friend class ObjectProtectedRef;

protected:
    Alma& alma;

private:
    static void protect_object(Alma& alma, GCObject* object);
    static void unprotect_object(Alma& alma, GCObject* object);

public:
    Object(Alma& alma);
    Object(const Object& other);
    Object(const Object&& other);

    virtual ObjectWeakRef<Object> eval(ObjectWeakRef<Object> self, ObjectWeakRef<Environment> enviroment);
    virtual ObjectWeakRef<Object> apply(
        ObjectWeakRef<Object> self,
        const std::vector<ObjectWeakRef<Object>>& arg_list,
        ObjectWeakRef<Environment> enviroment);
    virtual std::string to_string(ObjectWeakRef<Object> self);
    virtual bool typep(ObjectWeakRef<Object> self, ObjectWeakRef<Object> type);
    operator bool();
};

// -----------------------------------------------------------------------------

template <typename T>
class ObjectWeakRef {
    template <typename S>
    friend class ObjectRef;
    template <typename S>
    friend class ObjectWeakRef;
    template <typename S>
    friend class ObjectProtectedRef;
    template <typename S>
    friend class ObjectTrackedRef;
    friend struct ObjectRefHash;
    friend struct ObjectRefEqual;

public:
    template <typename S>
    using rebind = ObjectWeakRef<S>;
    using value_type = T;

private:
    GCObject* obj;

protected:
    Alma& alma;

public:
    ObjectWeakRef(const ObjectWeakRef& other);
    ObjectWeakRef(ObjectWeakRef&& other);
    template <ObjectRefType<T> S>
    ObjectWeakRef(S&& other);
    template <Related<T> S>
    ObjectWeakRef(Alma& alma, S* obj);
    ObjectWeakRef(std::nullptr_t) = delete;

    ObjectWeakRef& operator=(const ObjectWeakRef& other);
    ObjectWeakRef& operator=(ObjectWeakRef&& other);
    template <ObjectRefType<T> S>
    ObjectWeakRef& operator=(S&& other);
    template <Related<T> S>
    ObjectWeakRef& operator=(S* obj);
    ObjectWeakRef& operator=(std::nullptr_t) = delete;

    template <Related<T> S>
    ObjectWeakRef<S> as() const;

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

    operator bool() const;
};

template <typename T>
ObjectWeakRef<T>::ObjectWeakRef(const ObjectWeakRef& other)
    : obj(other.obj)
    , alma(other.alma)
{
}

template <typename T>
ObjectWeakRef<T>::ObjectWeakRef(ObjectWeakRef&& other)
    : obj(other.obj)
    , alma(other.alma)
{
}

template <typename T>
template <ObjectRefType<T> S>
ObjectWeakRef<T>::ObjectWeakRef(S&& other)
    : obj(other.obj)
    , alma(other.alma)
{
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>::ObjectWeakRef(Alma& _alma, S* _obj)
    : obj(_obj)
    , alma(_alma)
{
}

template <typename T>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(const ObjectWeakRef& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(ObjectWeakRef&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <ObjectRefType<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(S&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<T>& ObjectWeakRef<T>::operator=(S* _obj)
{
    this->obj = _obj;
    return *this;
}

template <typename T>
template <Related<T> S>
ObjectWeakRef<S> ObjectWeakRef<T>::as() const
{
    return ObjectWeakRef<S>(this->alma, static_cast<S*>(this->obj));
}

template <typename T>
T* ObjectWeakRef<T>::get()
{
    return static_cast<T*>(this->obj);
}

template <typename T>
T& ObjectWeakRef<T>::operator*()
{
    return *static_cast<T*>(this->obj);
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
    return *static_cast<T*>(this->obj);
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

template <typename T>
ObjectWeakRef<T>::operator bool() const
{
    return static_cast<bool>(*static_cast<Object*>(this->obj));
}

// -----------------------------------------------------------------------------

template <typename T>
class ObjectTrackedRef : public ObjectWeakRef<T> {
    friend struct ObjectRefHash;
    friend struct ObjectRefEqual;

public:
    template <typename S>
    using rebind = ObjectTrackedRef<S>;

private:
    Object& owner;

public:
    ObjectTrackedRef(const ObjectTrackedRef& other);
    ObjectTrackedRef(ObjectTrackedRef&& other);
    template <Related<T> S>
    ObjectTrackedRef(Object& owner, const ObjectTrackedRef<S>& other);
    template <Related<T> S>
    ObjectTrackedRef(Object& owner, ObjectTrackedRef<S>&& other);
    template <ObjectRefType<T> S>
    ObjectTrackedRef(Object& owner, S&& other);
    template <Related<T> S>
    ObjectTrackedRef(Object& owner, Alma& alma, S* obj);
    ObjectTrackedRef(std::nullptr_t) = delete;

    ~ObjectTrackedRef();

    ObjectTrackedRef& operator=(const ObjectTrackedRef& other);
    ObjectTrackedRef& operator=(ObjectTrackedRef&& other);
    template <ObjectRefType<T> S>
    ObjectTrackedRef& operator=(S&& other);
    template <Related<T> S>
    ObjectTrackedRef& operator=(S* obj);
    ObjectTrackedRef& operator=(std::nullptr_t) = delete;
};

template <typename T>
ObjectTrackedRef<T>::ObjectTrackedRef(const ObjectTrackedRef& other)
    : ObjectWeakRef<T>(other.alma, other.obj)
    , owner(other.owner)
{
    this->owner.track_reference(&this->obj);
}

template <typename T>
ObjectTrackedRef<T>::ObjectTrackedRef(ObjectTrackedRef&& other)
    : ObjectWeakRef<T>(other.alma, other.obj)
    , owner(other.owner)
{
    this->owner.track_reference(&this->obj);
}

template <typename T>
template <Related<T> S>
ObjectTrackedRef<T>::ObjectTrackedRef(Object& _owner, const ObjectTrackedRef<S>& other)
    : ObjectWeakRef<T>(other.alma, other.obj)
    , owner(_owner)
{
}

template <typename T>
template <Related<T> S>
ObjectTrackedRef<T>::ObjectTrackedRef(Object& _owner, ObjectTrackedRef<S>&& other)
    : ObjectWeakRef<T>(other.alma, other.obj)
    , owner(_owner)
{
}

template <typename T>
template <ObjectRefType<T> S>
ObjectTrackedRef<T>::ObjectTrackedRef(Object& _owner, S&& other)
    : ObjectWeakRef<T>(other)
    , owner(_owner)
{
    this->owner.track_reference(&this->obj);
}

template <typename T>
template <Related<T> S>
ObjectTrackedRef<T>::ObjectTrackedRef(Object& _owner, Alma& _alma, S* _obj)
    : ObjectWeakRef<T>(_alma, _obj)
    , owner(_owner)
{
    this->owner.track_reference(&this->obj);
}

template <typename T>
ObjectTrackedRef<T>::~ObjectTrackedRef()
{
    this->owner.untrack_reference(&this->obj);
}

template <typename T>
ObjectTrackedRef<T>& ObjectTrackedRef<T>::operator=(const ObjectTrackedRef& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
ObjectTrackedRef<T>& ObjectTrackedRef<T>::operator=(ObjectTrackedRef&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <ObjectRefType<T> S>
ObjectTrackedRef<T>& ObjectTrackedRef<T>::operator=(S&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <Related<T> S>
ObjectTrackedRef<T>& ObjectTrackedRef<T>::operator=(S* _obj)
{
    this->obj = _obj;
    return *this;
}

// --------------------------------------------------------------------------------

template <typename T>
class ObjectRef : public ObjectTrackedRef<T> {
    template <typename S>
    friend class ObjectWeakRef;
    template <typename S>
    friend class ObjectRef;
    template <typename S>
    friend class ObjectProtectedRef;
    friend struct ObjectRefHash;
    friend struct ObjectRefEqual;

public:
    template <typename S>
    using rebind = ObjectRef<S>;

public:
    template <ObjectRefType<T> S>
    ObjectRef(Object& owner, S&& other);
    template <Related<T> S>
    ObjectRef(Object& owner, Alma& alma, S* obj);
    ObjectRef(std::nullptr_t) = delete;

    ObjectRef& operator=(const ObjectRef& other);
    ObjectRef& operator=(ObjectRef&& other);
    template <ObjectRefType<T> S>
    ObjectRef& operator=(S&& other);
    template <Related<T> S>
    ObjectRef& operator=(S* obj);
    ObjectRef& operator=(std::nullptr_t) = delete;
};

template <typename T>
template <ObjectRefType<T> S>
ObjectRef<T>::ObjectRef(Object& _owner, S&& other)
    : ObjectTrackedRef<T>(_owner, std::forward<S>(other))
{
}

template <typename T>
template <Related<T> S>
ObjectRef<T>::ObjectRef(Object& _owner, Alma& _alma, S* _obj)
    : ObjectTrackedRef<T>(_owner, _alma, _obj)
{
}

template <typename T>
ObjectRef<T>& ObjectRef<T>::operator=(const ObjectRef& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
ObjectRef<T>& ObjectRef<T>::operator=(ObjectRef&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <ObjectRefType<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(S&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <Related<T> S>
ObjectRef<T>& ObjectRef<T>::operator=(S* _obj)
{
    this->obj = _obj;
    return *this;
}

// -----------------------------------------------------------------------------

template <typename T>
class ObjectProtectedRef : public ObjectWeakRef<T> {
    template <typename S>
    friend class ObjectRef;
    template <typename S>
    friend class ObjectWeakRef;
    template <typename S>
    friend class ObjectProtectedRef;
    friend struct ObjectRefHash;
    friend struct ObjectRefEqual;

public:
    template <typename S>
    using rebind = ObjectProtectedRef<S>;

public:
    ObjectProtectedRef(const ObjectProtectedRef& other);
    ObjectProtectedRef(ObjectProtectedRef&& other);
    template <ObjectRefType<T> S>
    ObjectProtectedRef(S&& other);
    template <Related<T> S>
    ObjectProtectedRef(Alma& alma, S* obj);
    ObjectProtectedRef(std::nullptr_t) = delete;

    ~ObjectProtectedRef();

    ObjectProtectedRef& operator=(const ObjectProtectedRef& other);
    ObjectProtectedRef& operator=(ObjectProtectedRef&& other);
    template <ObjectRefType<T> S>
    ObjectProtectedRef& operator=(S&& other);
    template <Related<T> S>
    ObjectProtectedRef& operator=(S* obj);
    ObjectProtectedRef& operator=(std::nullptr_t) = delete;
};

template <typename T>
ObjectProtectedRef<T>::ObjectProtectedRef(const ObjectProtectedRef& other)
    : ObjectWeakRef<T>(other)
{
    Object::protect_object(this->alma, this->obj);
}

template <typename T>
ObjectProtectedRef<T>::ObjectProtectedRef(ObjectProtectedRef&& other)
    : ObjectWeakRef<T>(other)
{
    Object::protect_object(this->alma, this->obj);
}

template <typename T>
template <ObjectRefType<T> S>
ObjectProtectedRef<T>::ObjectProtectedRef(S&& other)
    : ObjectWeakRef<T>(std::forward<S>(other))
{
    Object::protect_object(this->alma, this->obj);
}

template <typename T>
template <Related<T> S>
ObjectProtectedRef<T>::ObjectProtectedRef(Alma& _alma, S* _obj)
    : ObjectWeakRef<T>(_alma, _obj)
{
    Object::protect_object(this->alma, this->obj);
}

template <typename T>
ObjectProtectedRef<T>::~ObjectProtectedRef()
{
    Object::unprotect_object(this->alma, this->obj);
}

template <typename T>
ObjectProtectedRef<T>& ObjectProtectedRef<T>::operator=(const ObjectProtectedRef& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
ObjectProtectedRef<T>& ObjectProtectedRef<T>::operator=(ObjectProtectedRef&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <ObjectRefType<T> S>
ObjectProtectedRef<T>& ObjectProtectedRef<T>::operator=(S&& other)
{
    this->obj = other.obj;
    return *this;
}

template <typename T>
template <Related<T> S>
ObjectProtectedRef<T>& ObjectProtectedRef<T>::operator=(S* _obj)
{
    this->obj = _obj;
    return *this;
}

// --------------------------------------------------------------------------------

// Make ObjectRef and ObjectWeakRef hashable and transparent to be usable in std::unordered_map

struct ObjectRefHash {
    using is_transparent = void;

    template <ObjectRefType<Object> T>
    std::size_t operator()(const T& obj) const noexcept
    {
        return std::hash<GCObject*>()(obj.obj);
    }

    template <typename T>
    std::size_t operator()(const ObjectTrackedRef<T>& obj) const noexcept
    {
        return std::hash<GCObject*>()(obj.obj);
    }
};

struct ObjectRefEqual {
    using is_transparent = void;

    template <ObjectRefType<Object> T, ObjectRefType<Object> S>
    bool operator()(const T& obj1, const S& obj2) const noexcept
    {
        return obj1.obj == obj2.obj;
    }

    template <typename T, ObjectRefType<Object> S>
    bool operator()(const ObjectTrackedRef<T>& obj1, const S& obj2) const noexcept
    {
        return obj1.obj == obj2.obj;
    }

    template <ObjectRefType<Object> T, typename S>
    bool operator()(const T& obj1, const ObjectTrackedRef<S>& obj2) const noexcept
    {
        return obj1.obj == obj2.obj;
    }

    template <typename T, typename S>
    bool operator()(const ObjectTrackedRef<T>& obj1, const ObjectTrackedRef<S>& obj2) const noexcept
    {
        return obj1.obj == obj2.obj;
    }
};
