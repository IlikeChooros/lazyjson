#include "lazyjson.h"


BEGIN_LAZY_JSON_NAMESPACE

// ==============================
// stream implementation

void stream::set(char *data)
{
    if (data != "")
    {
        _data = data;
        _state = GOOB_BIT;
        _end = strlen(_data);
    }
    else
    {
        _state = EOF_BIT;
    }
    _pos = 0;
}

void stream::get(char &c)
{
    if (eof() || !_data[_pos])
    {
        _state = EOF_BIT;
        c = EOF_BIT;
    }
    else
    {
        c = _data[_pos];
        _pos++;
    }
}

void stream::seekg(size_t pos, stream_pos type)
{
    switch (type)
    {
    case stream_pos::base:
        _pos = pos;
        break;
    case stream_pos::cur:
        _pos += pos;
        break;
    default:
        break;
    }
    clear();
    if (_pos >= _end)
    {
        _state = EOF_BIT;
        _pos = _end;
    }
}

size_t stream::tellg()
{
    return _pos;
}

char stream::peek()
{
    if (good() && _data[_pos] != 0)
    {
        return _data[_pos];
    }
    return BAD_BIT;
}

size_t stream::size(){
    return _end;
}

char* stream::data(){
    return _data;
}

void stream::clear(size_t state)
{
    _state = state;
}

bool stream::eof()
{
    return (_state & EOF_BIT) != 0;
}

bool stream::bad()
{
    return (_state & BAD_BIT) != 0;
}

bool stream::good()
{
    return _state == GOOB_BIT;
}

// ==============================
// Tokenizer implementation

std::string verboseTokenType(TOKEN_TYPE type)
{
    switch (type)
    {
    case TOKEN_TYPE::CURLY_OPEN:
        return "CURLY_OPEN";
    case TOKEN_TYPE::CURLY_CLOSE:
        return "CURLY_CLOSE";
    case TOKEN_TYPE::ARRAY_OPEN:
        return "ARRAY_OPEN";
    case TOKEN_TYPE::ARRAY_CLOSE:
        return "ARRAY_CLOSE";
    case TOKEN_TYPE::COMMA:
        return "COMMA";
    case TOKEN_TYPE::COLON:
        return "COLON";
    case TOKEN_TYPE::STRING:
        return "STRING";
    case TOKEN_TYPE::NUMBER:
        return "NUMBER";
    case TOKEN_TYPE::BOOLEAN:
        return "BOOLEAN";
    case TOKEN_TYPE::NULL_TYPE:
        return "NULL_TYPE";
    default:
        return "UNKNOWN";
    }
}

Tokenizer::Tokenizer(const char *data)
{
    setData(data);
}

void Tokenizer::setData(const char *data)
{
    _prevPos = 0;
    _stream.set((char *)data);
}

std::string Tokenizer::json(int start, int end)
{
    if (end > _stream.size()){
        end = _stream.size();
    }
    while (end < 0){
        end += _stream.size();
    }
    if (start > end){
        std::swap(start, end);
    }
    return std::string(_stream.data() + start, end - start);
}

void Tokenizer::setPos(size_t pos)
{
    _stream.seekg(pos, stream_pos::base);
}

void Tokenizer::validatePos(size_t &pos)
{
    _prevPos = _stream.tellg();

    _stream.seekg(pos, stream_pos::base);
    static_cast<void>(getWithoutWhiteSpace());
    pos = _stream.tellg();

    _stream.seekg(_prevPos);
}

size_t Tokenizer::getPos()
{
    return _stream.tellg();
}

bool Tokenizer::isWhiteSpace(const char &c)
{
    return (c == ' ' || c == '\n');
}

bool Tokenizer::isNumber(const char &c)
{
    return c >= '0' && c <= '9';
}

bool Tokenizer::isPartOfNumber(const char &c)
{
    return c == '-' || isNumber(c);
}

bool Tokenizer::hasTokens()
{
    return !_stream.eof();
}

char Tokenizer::getWithoutWhiteSpace()
{
    char c = ' ';
    while (isWhiteSpace(c))
    {
        _stream.get(c);
        if (!_stream.good())
        {
            if (isWhiteSpace(c))
            {
                throw std::runtime_error("Tokenizer::getWithoutWhiteSpace(): Run out of tokens");
            }
            return c;
        }
    }
    return c;
}

void Tokenizer::rollBack()
{
    if (_stream.eof())
    {
        _stream.clear();
    }
    _stream.seekg(_prevPos);
}

Token Tokenizer::getToken()
{
    if (!hasTokens())
    {
        throw std::runtime_error("Tokenizer::getToken(): No more tokens");
    }
    _prevPos = _stream.tellg();
    char c = getWithoutWhiteSpace();

#if DEBUG_JSON
    Serial.printf("PARSING: %c \n", c);
#endif

    Token token;
    switch (c)
    {
    // string reading
    case '"':
        token.type = TOKEN_TYPE::STRING;
        // token.value = "";
        _stream.get(c);
        while (c != '"')
        {
            token.value += c;
            _stream.get(c);
        }
        break;
    case '{':
        token.type = TOKEN_TYPE::CURLY_OPEN;
        break;
    case '}':
        token.type = TOKEN_TYPE::CURLY_CLOSE;
        break;
    // Expecting false
    case 'f':
        token.type = TOKEN_TYPE::BOOLEAN;
        token.value = "false";
        // std::ios_base
        _stream.seekg(4, stream_pos::cur);
        break;
    // Expecting true
    case 't':
        token.type = TOKEN_TYPE::BOOLEAN;
        token.value = "true";
        _stream.seekg(3, stream_pos::cur);
        break;
    // Expecting null
    case 'n':
        token.type = TOKEN_TYPE::NULL_TYPE;
        _stream.seekg(3, stream_pos::cur);
        break;
    case '[':
        token.type = TOKEN_TYPE::ARRAY_OPEN;
        break;
    case ']':
        token.type = TOKEN_TYPE::ARRAY_CLOSE;
        break;
    case ':':
        token.type = TOKEN_TYPE::COLON;
        break;
    case ',':
        token.type = TOKEN_TYPE::COMMA;
        break;
    // Number / Float parsing
    /*
    Numbers contain only digits, dots, and minus sign:
    - . 0 1 2 3 4 5 6 7 8 9

    Number should not start with '.'
    */
    default:
        if (isPartOfNumber(c))
        {
            token.type = TOKEN_TYPE::NUMBER;

            char peek;
            while (!_stream.eof())
            {
                token.value += c;

                peek = _stream.peek();

#if DEBUG_JSON
                Serial.printf("Curr: %c Peek: %c \n", c, peek);
#endif

                // look one ahead to see if that's a number
                if (!(isPartOfNumber(peek) || peek == '.'))
                {
                    break;
                }
                _stream.get(c);
            }
        }
        else
        {
            // Unknown token / invalid syntax in given json
            throw std::runtime_error(std::string("Unexpected token: ") + c +  "("+ 
                std::to_string(int(c)) +  ") at: " + std::to_string(_prevPos));
        }
    }
    return token;
}

Token Tokenizer::peekToken()
{
    Token token = getToken();
    rollBack();
    return token;
}

// ==============================
// LazyObjects implementation


// This function is used to skip the tokens that are not needed
void fast_forward(size_t pos, TOKEN_TYPE begin, TOKEN_TYPE end, Tokenizer* _tokenizer)
{
    _tokenizer->setPos(pos);
    Token token;
    int count = 1;
    while (_tokenizer->hasTokens() && count > 0){
        token = _tokenizer->getToken();
        if (token.type == begin){
            count++;
        }
        if (token.type == end){
            count--;
        }
    }
}

std::string verboseLazyType(LazyType type){
    switch (type)
    {
    case LazyType::OBJECT:
        return "object";
    case LazyType::LIST:
        return "list";
    case LazyType::STRING:
        return "string";
    case LazyType::NUMBER:
        return "number";
    case LazyType::BOOL:
        return "boolean";
    case LazyType::NULL_TYPE:
        return "null";
    default:
        return "unknown";
    }
}

LazyObject *object_parse(size_t pos, bool deep, Tokenizer *_tokenizer)
{
    /*
    
    {
        "key1": "value1",
        "key2": 125.5,
        "key3": {
            "key4": "value4"
        }
    }
    
    The object stores only the keys of the object and the position of the value in the json string.
    When the value is needed, it is parsed and stored in the object. This way, we can parse only the
    values that are needed, and not the whole json string.

    When requesting a value, the object checks if the value is already parsed, and if not, it parses it
    and stores it in the object.

    So in this example, requesting key1 will parse "value1", and the whole value will be returned.
    But requesting key3 will only lazily parse the object, and not the value of key4.

    For example, let the obj = ... (the json string above)

        std::string s = obj["key1"]; 
        // this will result in parsing "value1" and returning it

        std::string s1 = obj["key3"]["key4"];
        // this will result in parsing object at key3, and then parsing the value of key4,
        // the result of the "key3" will be cached, so it will not be parsed again.

        std::string s2 = obj["key3"]["key4"];
        // this will result in returning the cached value of key4, and not parsing it again.

    Caution:
        Only numbers and boolean values are stored as values, so when
        accessing them, the value is returned immediately,
        and not lazily parsed.

        Other values are also cached, but only after the first access.

        Calling obj["key2"] will return 125.5 immediately, and not lazily parse it.
        But obj["key1"] will first lazily parse the value, and when called again
        it will return the cached value.

    Also accessing the keys in object is done in linear time, so it is recomended
    to store the results in a variable, and not access the same key multiple times.

    Calling:
        obj["key1"]["key2"]["key3"]

        Multiple times will result in searching the key1, key2 and key3 multiple times.
        
    */

    _tokenizer->setPos(pos);
    Token token;
    auto object = new LazyObject(_tokenizer);

    while (_tokenizer->hasTokens()){
        token = _tokenizer->getToken();

    #if DEBUG_LAZY_JSON
        Serial.printf("Parsing object token %s \n",
            verboseTokenType(token.type).c_str());
    #endif

        if (token.type == TOKEN_TYPE::COMMA){
            continue;
        }
        if (token.type == TOKEN_TYPE::CURLY_CLOSE){
            break; 
        }            
        if (token.type != TOKEN_TYPE::STRING){   
            break;
        }
        auto key = token.value;
    #if DEBUG_LAZY_JSON
        Serial.printf("Parsing object key %s \n", key.c_str());
    #endif
        if (_tokenizer->getToken().type != TOKEN_TYPE::COLON){
            break;
        }
        
        if(deep){
            auto parsed = lazy_parse(_tokenizer->getPos(), true, _tokenizer);
            object->add(key, parsed.values, parsed.type);
            continue;
        } 

        // storing the key and the position of the value
        object->add(key, _tokenizer->getPos());
        // value of the key is not parsed yet, so we need to skip it
        token = _tokenizer->getToken();

    #if DEBUG_LAZY_JSON
        Serial.printf("Parsing object value %s = %s \n",
            verboseTokenType(token.type).c_str(), token.value.c_str());
    #endif
        // nested objects are not evaluated, so we need to skip them
        if (token.type == TOKEN_TYPE::CURLY_OPEN){
            fast_forward(_tokenizer->getPos(), TOKEN_TYPE::CURLY_OPEN, TOKEN_TYPE::CURLY_CLOSE, _tokenizer);
        }
        // lists are not evaluated, so we need to skip them
        if (token.type == TOKEN_TYPE::ARRAY_OPEN){
            fast_forward(_tokenizer->getPos(), TOKEN_TYPE::ARRAY_OPEN, TOKEN_TYPE::ARRAY_CLOSE, _tokenizer);
        }
    }
    object->push(pos, _tokenizer->getPos());
    return object;
}

LazyList *list_parse(size_t pos, bool deep, Tokenizer *_tokenizer)
{
    /*
    
    [
        "value1",
        125.5,
        {
            "key4": "value4"
        }
    ]

    The list stores only the index of the value in the json string.
    When the value is needed, it is parsed and stored in the list.
    This way, we can parse only the values that are needed,
    and not the whole json string.

    When requesting a value, the list checks if the value is already parsed, and if not, it parses it
    and stores it in the list.
    
    For example, let the list = ... (json string above)
        list[0] will parse "value1" and return it
        list[1] will parse 125.5 and return it 
        list[2] will parse the object at index 2, and then parse the value of key4

    Accessing again list[0], will result in returning the cached value, and not parsing it again.

    Booleans and numbers are stored as values, so they are not parsed again.
    (Since lazy objects are more expensive than just an float and a boolean)

    */

    _tokenizer->setPos(pos);
    Token token;
    auto list = new LazyList(_tokenizer);
    int index = 0;
    size_t prev_pos;

    while (_tokenizer->hasTokens()){

        prev_pos = _tokenizer->getPos();
        token = _tokenizer->getToken();

    #if DEBUG_LAZY_JSON
        Serial.printf("Parsing list token %s = %s\n",
            verboseTokenType(token.type).c_str(), token.value.c_str());
    #endif

        if (token.type == TOKEN_TYPE::ARRAY_CLOSE){
            break; 
        }
        if (token.type == TOKEN_TYPE::COMMA){
            continue;
        }

        if(deep){
            auto parsed = lazy_parse(_tokenizer->getPos(), true, _tokenizer);
            list->add(index, parsed.values, parsed.type);
            continue;
        } 

        list->add(index, prev_pos);
        index++;

        // nested objects are not evaluated, so we need to skip them
        if (token.type == TOKEN_TYPE::CURLY_OPEN){
            fast_forward(_tokenizer->getPos(), TOKEN_TYPE::CURLY_OPEN, TOKEN_TYPE::CURLY_CLOSE, _tokenizer);
        }
        // lists are not evaluated, so we need to skip them
        if (token.type == TOKEN_TYPE::ARRAY_OPEN){
            fast_forward(_tokenizer->getPos(), TOKEN_TYPE::ARRAY_OPEN, TOKEN_TYPE::ARRAY_CLOSE, _tokenizer);
        }
    }
    list->push(pos, _tokenizer->getPos());
    return list;
}


LazyString *string_parse(size_t pos, Tokenizer *_tokenizer)
{
    /*
    
    "value1"

    The LazyString stores only the position of the value in the json string.
    When the value is needed, it is copied from json and returned, but the
    copied string itself is not stored in LazyString.

    So for example:
        LazyString *s = string_parse(pos);
        // the string is parsed, the beginning and the end of the string is stored

        std::string str = s->str();
        // will result in copying the substring of the json

    */
    return new LazyString(pos, _tokenizer->getPos(), _tokenizer);
}

LazyTypedValues lazy_parse(size_t pos, bool deep, Tokenizer *_tokenizer)
{
    _tokenizer->setPos(pos);
    auto token = _tokenizer->getToken();
    LazyTypedValues result;

    switch (token.type)
    {
    case TOKEN_TYPE::CURLY_OPEN:
        result.values.object = object_parse(_tokenizer->getPos(), deep, _tokenizer);
        result.type = LazyType::OBJECT;
        break;
    case TOKEN_TYPE::ARRAY_OPEN:
        result.values.list = list_parse(_tokenizer->getPos(), deep, _tokenizer);
        result.type = LazyType::LIST;
        break;
    case TOKEN_TYPE::STRING:
        result.values.string = string_parse(pos, _tokenizer);
        result.type = LazyType::STRING;
        break;
    case TOKEN_TYPE::NUMBER:
        result.values.number = std::stof(token.value);
        result.type = LazyType::NUMBER;
        break;
    case TOKEN_TYPE::BOOLEAN:
        result.values.boolean = token.value == "true";
        result.type = LazyType::BOOL;
        break;
    case TOKEN_TYPE::NULL_TYPE:
        result.type = LazyType::NULL_TYPE;
        break;
    default:
        break;
    }
    return result;
}

void destroyLazyValue(LazyValues& value, LazyType& type)
{
    switch (type)
    {
    case LazyType::OBJECT:

    #if DEBUG_LAZY_JSON
        Serial.printf("Destroying lazy object at %p \n", value.object);
    #endif
        
        delete value.object;
        break;
    case LazyType::LIST:
    #if DEBUG_LAZY_JSON
        Serial.printf("Destroying lazy list at %p \n", value.list);
    #endif
        delete value.list;
        break;
    case LazyType::STRING:
    #if DEBUG_LAZY_JSON
        Serial.printf("Destroying lazy string at %p \n", value.string);
    #endif
        delete value.string;
        break;        
    default:
    #if DEBUG_LAZY_JSON
        Serial.printf("Destroying: Unknown lazy type %d \n", type);
    #endif
        break;
    }
}

void deepCopyLazyValue(LazyValues& value, LazyType& type, LazyValues& dest)
{
    switch (type)
    {
    case LazyType::OBJECT:
        dest.object = new LazyObject(*value.object);
        break;
    case LazyType::LIST:
        dest.list = new LazyList(*value.list);
        break;
    case LazyType::STRING:
        dest.string = new LazyString(*value.string);
        break;        
    case LazyType::NUMBER:
        dest.number = value.number;
        break;
    case LazyType::BOOL:
        dest.boolean = value.boolean;
        break;
    default:
        break;
    }
}

// LazyLike

std::string LazyLike::json(){
    return _tokenizer->json(_start, _end);
}

void LazyLike::push(int start, int end){
    _start = start;
    _end = end;
}

// LazyObject

LazyObject::LazyObject(Tokenizer *t): LazyLike(t) {}

LazyObject::LazyObject(const LazyObject& other){
    static_cast<void>(this->operator=(other));
}

LazyObject& LazyObject::operator=(const LazyObject& other)
{
    _start = other._start;
    _end = other._end;
    _tokenizer = other._tokenizer;
    for(auto it = other._list.begin(); it != other._list.end(); it++){
        ObjectData data;
        auto values = it->values;
        auto type = it->type;
        data.key = it->key;
        data.type = type;            
        deepCopyLazyValue(values, type, data.values);
        _list.push_back(data);
    }
    return *this;
}

LazyObject::~LazyObject(){
#if DEBUG_LAZY_JSON
    Serial.println("Destroying LazyObject \n");
#endif
    for (auto it = _list.begin(); it != _list.end(); it++){
        destroyLazyValue(it->values, it->type);
    }
}

void LazyObject::add(const std::string& key, int parse_idx)
{
    ObjectData data;
    data.key = key;
    data.values.parse_idx = parse_idx;
    data.type = LazyType::PARSE_IDX;
    _list.push_back(data);
    
#if DEBUG_LAZY_JSON
    Serial.printf("Added %s at %d \n", key.c_str(), parse_idx);
#endif
}

void LazyObject::add(const std::string& key, LazyValues value, LazyType type)
{
    ObjectData data;
    data.key = key;
    data.values = value;
    data.type = type;
    _list.push_back(data);
}

LazyTypedValues LazyObject::operator[](const std::string& key)
{
    return get(key, false);
}

LazyTypedValues LazyObject::get(const std::string& key, bool cache)
{
#if DEBUG_LAZY_JSON
    Serial.printf("Getting %s \n", key.c_str());
#endif
    LazyTypedValues result;
    for (auto it = _list.begin(); it != _list.end(); it++){
#if DEBUG_LAZY_JSON
        Serial.printf("Comparing %s with %s \n", key.c_str(), it->key.c_str());
#endif
        if (it->key == key){
            // If the value is not parsed, we need to parse it
            if (it->type == LazyType::PARSE_IDX){

#if DEBUG_LAZY_JSON
                Serial.printf("Parsing %s at %d \n", key.c_str(), it->values.parse_idx);
#endif
                result = lazy_parse(it->values.parse_idx, cache, _tokenizer);
                it->values = result.values;
                it->type = result.type;
            } else {
                // Already parsed, we can return the value
                result.values = it->values;
                result.type = it->type;
            }
            break;
        }
    }
    return result;
}


// LazyList

LazyList::LazyList(Tokenizer *t): LazyLike(t) {}

LazyList::LazyList(const LazyList& other){
    static_cast<void>(this->operator=(other));
}

LazyList& LazyList::operator=(const LazyList& other)
{
    _start = other._start;
    _end = other._end;
    _tokenizer = other._tokenizer;
    for(auto it = other._list.begin(); it != other._list.end(); it++){
        ListData data;
        auto values = it->values;
        auto type = it->type;
        data.index = it->index;
        data.type = type;            
        deepCopyLazyValue(values, type, data.values);
        _list.push_back(data);
    }
    return *this;
}

LazyList::~LazyList(){
#if DEBUG_LAZY_JSON
    Serial.println("Destroying LazyList \n");
#endif
    for (auto it = _list.begin(); it != _list.end(); it++){
        destroyLazyValue(it->values, it->type);
    }
}

void LazyList::add(int index, int parse_idx)
{
    ListData data;
    data.index = index;
    data.values.parse_idx = parse_idx;
    data.type = LazyType::PARSE_IDX;
    _list.push_back(data);
}

void LazyList::add(int index, LazyValues value, LazyType type)
{
    ListData data;
    data.index = index;
    data.values = value;
    data.type = type;
    _list.push_back(data);
}

LazyTypedValues LazyList::operator[](const int& index)
{
    return get(index, false);
}

LazyTypedValues LazyList::get(const int& index, bool cache)
{
#if DEBUG_LAZY_JSON
    Serial.printf("Getting %i \n", index);
#endif
    LazyTypedValues result;
    for (auto it = _list.begin(); it != _list.end(); it++){
#if DEBUG_LAZY_JSON
        Serial.printf("Comparing %i with %i \n", index, it->index);
#endif
        if (it->index == index){
            // If the value is not parsed, we need to parse it
            if (it->type == LazyType::PARSE_IDX){
#if DEBUG_LAZY_JSON
                Serial.printf("Parsing %i at %d \n", index, it->values.parse_idx);
#endif
                result = lazy_parse(it->values.parse_idx, cache, _tokenizer);
                it->values = result.values;
                it->type = result.type;
            } else {
                // Already parsed, we can return the value
                result.values = it->values;
                result.type = it->type;
            }
            break;
        }
    }

#if DEBUG_LAZY_JSON
    Serial.printf("Returning %s", verboseLazyType(result.type).c_str());
#endif

    return result;
}


// LazyString
LazyString::LazyString(const LazyString& other){
    static_cast<void>(this->operator=(other));
}

LazyString& LazyString::operator=(const LazyString& other)
{
    _start = other._start;
    _end = other._end;
    _tokenizer = other._tokenizer;
    return *this;
}

std::string LazyString::str(){
    size_t start = _start;
    _tokenizer->validatePos(start);
    _start = start;
#if DEBUG_LAZY_JSON
    Serial.printf("Representing string: %s \n", _tokenizer->json(_start, _end-1).c_str());
#endif
    return _tokenizer->json(_start, _end - 1);
}


// ==============================
// Wrapper

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


// ==============================
// Errors

std::string lazyTypeError(const LazyTypedValues& node, LazyType type){
    return "expected " + verboseLazyType(type) + " but got " + verboseLazyType(node.type);
}

context_not_found::context_not_found(const std::string& key)
    : std::runtime_error("context not found: " + key)
{}

index_out_of_range::index_out_of_range(int index)
    : std::runtime_error("index out of range: " + std::to_string(index))
{}

invalid_type::invalid_type(const LazyType& expected, LazyType type)
    : std::runtime_error("expected " + verboseLazyType(expected) + " but got " + verboseLazyType(type))
{}



// ==============================
// Extractor



extractor::extractor(const char *json)
{
    static_cast<void>(use(json));
}

extractor::~extractor() {}

void extractor::reset()
{
    _start = 0;
    _end = -1;
}

extractor &extractor::reset_cache()
{
    _cache_start = _start;
    _end = -1;
    return *this;
}

void extractor::cache()
{
    _start = _cache_start;

    // get the end of the value
    _tokenizer.setPos(_start);

    Token token = _tokenizer.getToken();

    if (token.type == TOKEN_TYPE::CURLY_OPEN){
        fast_forward(_tokenizer.getPos(), TOKEN_TYPE::CURLY_OPEN,
            TOKEN_TYPE::CURLY_CLOSE, &_tokenizer);
    }
    else if(token.type == TOKEN_TYPE::ARRAY_OPEN){
        fast_forward(_tokenizer.getPos(), TOKEN_TYPE::ARRAY_OPEN,
            TOKEN_TYPE::ARRAY_CLOSE, &_tokenizer);
    }
    // else the values are parsed as a whole (strings, numbers, booleans, nulls)
    _end = (int)_tokenizer.getPos();

    _tokenizer.setPos(_start);
    _json = _tokenizer.json(_start, _end);
}

extractor &extractor::use(const char *json)
{
    _tokenizer.setData(json);
    _start = 0;
    _end = -1;
    _cache_start = 0;
    return *this;
}

const std::string &extractor::json()
{
    return _json;
}

void extractor::_validate(const LazyType &expected)
{
    Token token = _tokenizer.peekToken();

    bool invalid = false;
    LazyType valueType;

    switch (token.type)
    {
    case TOKEN_TYPE::STRING:
        valueType = LazyType::STRING;
        break;
    case TOKEN_TYPE::CURLY_OPEN:
        valueType = LazyType::OBJECT;
        break;
    case TOKEN_TYPE::ARRAY_OPEN:
        valueType = LazyType::LIST;
        break;
    case TOKEN_TYPE::NUMBER:
        valueType = LazyType::NUMBER;
        break;
    case TOKEN_TYPE::BOOLEAN:
        valueType = LazyType::BOOL;
        break;
    case TOKEN_TYPE::NULL_TYPE:
        valueType = LazyType::NULL_TYPE;
        break;
    default:
        throw std::runtime_error("Unknown type");
        break;
    }

    if (expected != valueType){
        throw invalid_type(expected, valueType);
    }
}

wrapper extractor::extract(bool reset_cache)
{
    wrapper w(lazy_parse(_cache_start, false, &_tokenizer));
    
    if(reset_cache){
        static_cast<void>(this->reset_cache());
    }
    // reset the tokenizer to the start of the value
    _tokenizer.setPos(_cache_start);

    return w;
}

extractor &extractor::filter(const std::string &find)
{
    _tokenizer.setPos(_cache_start);
    _validate(LazyType::OBJECT);
    // curly open token
    _tokenizer.getToken();
    
    Token token;

#if DEBUG_LAZY_JSON
    Serial.printf("Extractor: Filtering %s, json = %s\n", find.c_str(), _tokenizer._stream.data());
#endif

    while (_tokenizer.hasTokens()){
        // key of the object
        token = _tokenizer.getToken();

    #if DEBUG_LAZY_JSON
        Serial.printf("Extractor: Parsing object token %s = %s\n",
            verboseTokenType(token.type).c_str(), token.value.c_str());
    #endif

        if (token.type == TOKEN_TYPE::COMMA){
            continue;
        }
        if (token.type == TOKEN_TYPE::CURLY_CLOSE){
            break; 
        }        

        // key must be a string    
        if (token.type != TOKEN_TYPE::STRING){   
            break;
        }
        auto key = token.value;

    #if DEBUG_LAZY_JSON
        Serial.printf("Extractor: Parsing object key %s \n", key.c_str());
    #endif
        // colon must be next
        if (_tokenizer.getToken().type != TOKEN_TYPE::COLON){
            break;
        }

        // if the key is found, store the position of the value
        if (find == key){
            // store the position of the value, prepare for the next parsing
            _cache_start = static_cast<int>(_tokenizer.getPos());

#if DEBUG_LAZY_JSON
    Serial.printf("Extractor: Found %s at %i\n", find.c_str(), _cache_start);
#endif
    
            return *this;
        }
        // value of the key is not parsed yet, so we need to skip it
        token = _tokenizer.getToken();

    #if DEBUG_LAZY_JSON
        Serial.printf("Extractor: Parsing object value %s = %s \n",
            verboseTokenType(token.type).c_str(), token.value.c_str());
    #endif
        // nested objects are not evaluated, so we need to skip them
        if (token.type == TOKEN_TYPE::CURLY_OPEN){
            fast_forward(_tokenizer.getPos(), TOKEN_TYPE::CURLY_OPEN, TOKEN_TYPE::CURLY_CLOSE, &_tokenizer);
        }
        // lists are not evaluated, so we need to skip them
        else if (token.type == TOKEN_TYPE::ARRAY_OPEN){
            fast_forward(_tokenizer.getPos(), TOKEN_TYPE::ARRAY_OPEN, TOKEN_TYPE::ARRAY_CLOSE, &_tokenizer);
        }
    }

    return *this;
}

extractor &extractor::filter(int index){
    _tokenizer.setPos(_cache_start);
    _validate(LazyType::LIST);
    // array open token
    _tokenizer.getToken();
    int i = 0, value_pos = 0;
    Token token;

    while (_tokenizer.hasTokens()){

        // value of the list
        value_pos = static_cast<int>(_tokenizer.getPos());
        token = _tokenizer.getToken();

    #if DEBUG_LAZY_JSON
        Serial.printf("Extractor: Parsing list token %s \n",
            verboseTokenType(token.type).c_str());
    #endif

        if (token.type == TOKEN_TYPE::COMMA){
            continue;
        }
        if (token.type == TOKEN_TYPE::ARRAY_CLOSE){
            break; 
        }     

        // if the index is found, store the position of the value
        if (i == index){
            // store the position of the value, prepare for the next parsing
            _cache_start = value_pos;
#if DEBUG_LAZY_JSON
    Serial.printf("Found %i at %i\n", index, _cache_start);
#endif
            return *this;
        }   

        // value of the key is not parsed yet, so we need to skip it
        if (token.type == TOKEN_TYPE::CURLY_OPEN){
            fast_forward(_tokenizer.getPos(), TOKEN_TYPE::CURLY_OPEN, TOKEN_TYPE::CURLY_CLOSE, &_tokenizer);
        }
        else if (token.type == TOKEN_TYPE::ARRAY_OPEN){
            fast_forward(_tokenizer.getPos(), TOKEN_TYPE::ARRAY_OPEN, TOKEN_TYPE::ARRAY_CLOSE, &_tokenizer);
        }
        i++;
    }

    return *this;
}

extractor& extractor::operator[](const std::string& key){
    return filter(key);
}

extractor& extractor::operator[](int index){
    return filter(index);
}



END_LAZY_JSON_NAMESPACE