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
    bool asNull();
};

END_LAZY_JSON_NAMESPACE
