#pragma once

/*

The whole idea of extracor is to filter the json string and store the positions of
the elements that are going to be parsed. This way, the parsing is done only when
the value is accessed. This is useful when the json string is big and only a few
elements are going to be used.

*/


#include <string>

#include "wrappers.h"


BEGIN_LAZY_JSON_NAMESPACE

/// @brief Class used to extract values from a json string. Parsing is done only
/// when the value is accessed (either by [] operator or by calling `extract()`).
class extractor
{
    std::string _json;
    int _start;
    int _end;
    int _cache_start;
    Tokenizer _tokenizer;

    void _validate(const LazyType &expected);
public:
    extractor(const char *json);
    ~extractor();

    /// @brief Resets the start and end positions of the parsing.
    void reset();

    /// @brief Resets the cache start position.
    extractor &reset_cache();

    /// @brief Sets the json string to be used for parsing.
    extractor &use(const char *json);

    /// @brief Caches the current json string. This is basically a substring of the
    /// json string, so you can use the cached string to parse the other values later.
    /// Minimizing the number of times the json string is parsed. To get the cached json call
    /// `json()`. To load the cached string, use the `use(const char* json)` method.
    void cache();

    /// @brief Returns the cached json string.
    const std::string& json();

    /// @brief Filters the JSON string by a key, the result is not extracted (parsed), 
    /// to get the value use the `extract()` method. If the key is not found, nothing happens,
    /// calling `extract()` will return this json object (since this is the value that was filtered)
    /// @param key 
    /// @throw `json::lazy::invalid_type` if the value is not an object.
    /// @return *this
    extractor &filter(const std::string &key);

    /// @brief Filters the JSON string by an index, the result is not extracted (parsed), 
    /// to get the value use the `extract()` method. If the index is out of range, nothing happens,
    /// calling `extract()` will return this json list (since this is the value that was filtered)
    /// @param index
    /// @throw `json::lazy::invalid_type` if the value is not a list.
    /// @return *this
    extractor &filter(int index);

    /// @brief Filters the JSON string by a key, same as `filter(const std::string &key)`
    extractor &operator[](const std::string &key);

    /// @brief Filters the JSON string by an index, same as `filter(int index)`
    extractor &operator[](int index);

    /// @brief This method is used to parse the json string and return the values.
    /// @param reset_cache If true, the cache start position will be reset to the start of the json string.
    /// See `reset_cache()` and `cache()` methods.
    /// @return Parsing result.
    wrapper extract(bool reset_cache = true);
};


END_LAZY_JSON_NAMESPACE