#include "wrappers.h"


BEGIN_LAZY_JSON_NAMESPACE

wrapper::wrapper(LazyTypedValues value) {
    this->_value = value;
}

wrapper::wrapper(const wrapper& other) {
    static_cast<void>(operator=(other));
}

wrapper& wrapper::operator=(const wrapper& other) {

#if DEBUG_LAZY_JSON
    Serial.println("Copying wrapper");
#endif

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

bool wrapper::asNull(){
    if(_value.type != LazyType::NULL_TYPE){
        throw invalid_type(LazyType::NULL_TYPE, _value.type);
    }
    return true;
}

std::string wrapper::asString(){
    if(_value.type != LazyType::STRING){
        throw invalid_type(LazyType::STRING, _value.type);
    }
    return _value.values.string->str();
}

END_LAZY_JSON_NAMESPACE