#include "wrappers.h"


BEGIN_LAZY_JSON_NAMESPACE


wrapper::wrapper(LazyTypedValues value) {
    this->_value = value;
}

wrapper::wrapper(const wrapper& other) {
    static_cast<void>(operator=(other));
}

wrapper& wrapper::operator=(const wrapper& other) {
    auto values = other._value.values;
    auto type = other._value.type;
    // clean up the current values
    destroyLazyValue(_value.values, _value.type);
    // copy the new values
    deepCopyLazyValue(values, type, _value.values);
    _value.type = type;
    return *this;
}

wrapper::~wrapper(){
#if DEBUG_LAZY_JSON
    Serial.println("Destroying wrapper");
#endif
    destroyLazyValue(_value.values, _value.type);
}

LazyType wrapper::type(){
    return _value.type;
}

LazyTypedValues& wrapper::raw(){
    return _value;
}

LazyObject& wrapper::object(){
    if (_value.type != LazyType::OBJECT){
        throw invalid_type(LazyType::OBJECT, _value.type);
    }
    return *(_value.values.object);
}

LazyList& wrapper::list(){
    if(_value.type != LazyType::LIST){
        throw invalid_type(LazyType::LIST, _value.type);
    }
    return *(_value.values.list);
}

int wrapper::asInt(){
    if(_value.type != LazyType::NUMBER){
        throw invalid_type(LazyType::NUMBER, _value.type);
    }
    return static_cast<int>(_value.values.number);
}

float wrapper::asFloat(){
    if(_value.type != LazyType::NUMBER){
        throw invalid_type(LazyType::NUMBER, _value.type);
    }
    return _value.values.number;
}

bool wrapper::asBool(){
    if(_value.type != LazyType::BOOL){
        throw invalid_type(LazyType::BOOL, _value.type);
    }
    return _value.values.boolean;
}

std::string wrapper::asString(){
    if(_value.type != LazyType::STRING){
        throw invalid_type(LazyType::STRING, _value.type);
    }
    return _value.values.string->str();
}


data::data(LazyTypedValues value) {
    static_cast<void>(init(value));
}

data::data(LazyValues value, LazyType type) {
    static_cast<void>(init({.values = value, .type = type}));
}

data& data::init(LazyTypedValues value) {
    this->value = value;
    return *this;
}

data::~data(){
#if DEBUG_LAZY_JSON
    Serial.println("Destroying data");
#endif
    switch (value.type)
    {
    case LazyType::OBJECT:

#if DEBUG_LAZY_JSON
    Serial.printf("Destroying object at %p \n", value.values.object);
#endif
        delete value.values.object;
        break;
    case LazyType::LIST:

#if DEBUG_LAZY_JSON
    Serial.printf("Destroying list at %p \n", value.values.list);
#endif
        delete value.values.list;
        break;
    case LazyType::STRING:

#if DEBUG_LAZY_JSON
    Serial.printf("Destroying string at %p \n", value.values.string);
#endif
        delete value.values.string;
        break;
    default:
        break;
    }
}

LazyType data::type(){
    return value.type;
}

const LazyValues& data::values(){
    return value.values;
}

LazyObject& data::object(){
    return *value.values.object;
}

std::string data::string(){
    return value.values.string->str();
}

LazyList& data::list(){
    return *value.values.list;
}

float data::number(){
    return value.values.number;
}

bool data::boolean(){
    return value.values.boolean;
}


END_LAZY_JSON_NAMESPACE