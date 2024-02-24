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
    _assert_type(LazyType::OBJECT);
    return *(_value.values.object);
}

LazyList& wrapper::list(){
    _assert_type(LazyType::LIST);
    return *(_value.values.list);
}

int wrapper::asInt(){
    _assert_type(LazyType::NUMBER);
    return static_cast<int>(_value.values.number);
}

float wrapper::asFloat(){
    _assert_type(LazyType::NUMBER);
    return _value.values.number;
}

bool wrapper::asBool(){
    _assert_type(LazyType::BOOL);
    return _value.values.boolean;
}

bool wrapper::isNull(){
    return _value.type == LazyType::NULL_TYPE;
}

std::string wrapper::asString(){
    _assert_type(LazyType::STRING);
    return _value.values.string->str();
}

void wrapper::_assert_type(LazyType type){
    if(_value.type != type){
        this->~wrapper();
        throw invalid_type(type, _value.type);
    }
}

END_LAZY_JSON_NAMESPACE