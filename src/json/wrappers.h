#pragma once

#include "errors.h"
#include "objects.h"
#include <type_traits>

#include <Arduino.h>

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
    inline T as();

    int asInt();
    float asFloat();
    bool asBool();
    String asString();
    bool isNull();
};

template<typename T>
inline T wrapper::as()
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

template<>
inline String wrapper::as<String>(){
    return asString();
}

template<>
inline std::string wrapper::as<std::string>(){
    _assert_type(LazyType::STRING);
    return _value.values.string->str();
}

END_LAZY_JSON_NAMESPACE
