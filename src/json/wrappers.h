#pragma once

#include "errors.h"
#include "objects.h"
#include <type_traits>

BEGIN_LAZY_JSON_NAMESPACE



class wrapper
{
    LazyTypedValues _value;
    void _assert_type(LazyType type);
public:
    wrapper() = default;
    wrapper(LazyTypedValues init);
    wrapper(const wrapper& other);
    ~wrapper();

    wrapper& operator=(const wrapper& other);

    LazyType type();
    LazyTypedValues& raw();

    LazyObject& object();
    LazyList& list();

    template<typename T>
    T as();

    int asInt();
    float asFloat();
    bool asBool();
    std::string asString();
    bool isNull();
};

template<typename T>
T wrapper::as()
{
    static_assert(std::is_arithmetic<T>::value, "wrapper::as<T>() : Type T must be an arithmetic type (int, bool, float, etc.)");
    if (std::is_same<T, bool>::value)
    {
        return asBool();
    }
    else if (std::is_arithmetic<T>::value)
    {
        _assert_type(LazyType::NUMBER);
        return static_cast<T>(_value.values.number);
    }
}

END_LAZY_JSON_NAMESPACE
