#pragma once

#include "errors.h"
#include "objects.h"

BEGIN_LAZY_JSON_NAMESPACE

class wrapper
{
    LazyTypedValues _value;
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

    int asInt();
    float asFloat();
    bool asBool();
    std::string asString();

};

/// @brief A wrapper for the LazyTypedValues struct, deletes the allocated memory when the object is destroyed
class data
{
    
public:
    data() = default;
    data(LazyTypedValues init);
    data(LazyValues init, LazyType type);
    ~data();

    data& init(LazyTypedValues init);

    LazyType type();
    const LazyValues& values();

    LazyObject& object();
    std::string string();
    LazyList& list();
    float number();
    bool boolean();

    LazyTypedValues value;
};

END_LAZY_JSON_NAMESPACE
