#include "extractor.h"


BEGIN_LAZY_JSON_NAMESPACE


extractor::extractor(const char *json)
{
    static_cast<void>(set(json));
}

extractor::~extractor() {}

void extractor::reset()
{
    _start = 0;
    _reset_cache();
    const char *data = _data;
    static_cast<void>(set(data));
}

void extractor::_reset_cache()
{
    _cache_start = _start;
    _end = -1;
}

void extractor::cache()
{
    // get the end of the value
    _tokenizer.setPos(_cache_start);

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
    _json = _tokenizer.json(_cache_start, _end);

#if DEBUG_LAZY_JSON
    Serial.printf("Extractor: Caching %s\n", _json.c_str());
#endif
    _set_cache();
}

extractor &extractor::set(const char *json)
{
    _data = const_cast<char *>(json);
    _tokenizer.setData(json);
    _start = 0;
    _end = -1;
    _cache_start = 0;
    return *this;
}

void extractor::_set_cache()
{
    _start = 0;
    _end = -1;
    _cache_start = 0;
    _tokenizer.setData(_json.c_str());
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

wrapper extractor::extract()
{
    wrapper w(lazy_parse(_cache_start, false, &_tokenizer));
    _reset_cache();
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