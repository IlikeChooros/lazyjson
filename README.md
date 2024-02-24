# Lazy JSON Extraction Library

This library provides a simple and efficient way to extract JSON data in C++. It's designed to be lightweight and easy to use, making it ideal for resource-constrained environments such as Arduino.

## Features

- **Lazy Extraction**: Extract only the data you need, when you need it. This can significantly reduce memory usage and improve performance when working with large JSON documents.
- **Easy to Use**: The library provides a simple and intuitive API. You can extract data using the `[]` operator and string keys or integer indices.
- **Error Handling**: The library includes robust error handling to help you catch and handle errors in your JSON data.

## How to Use

### Include the Library

Include the `lazyjson.h` file in your project.

```cpp
#include <lazyjson.h>
```

### Create an Extractor

Create an `extractor` object with your JSON data.

```cpp
extractor ex("{\"version\": [\"lazyjson\", 1.0], \"key\": true}");
```

### Extract Data

You can extract data using the `[]` operator. Use a string key to extract a value from an object, or an integer index to extract a value from an array.

```cpp
std::string value = ex["key"].extract().asBool(); // true
int value = ex["version"][1].extract().asInt(); // 1
```

You can also chain `[]` operators to extract nested data.

```cpp
std::string value = ex["key"]["nested_key"].extract().asString();
```


## Caching

The `extractor` class allows caching current value (as a json string).
To improve performance you can use this caching technique when extracting data from the same object or list.
The effects drastically magnify while working with bigger structures.


Here's an example of how to use the `cache()` method:

```cpp
// Create an extractor object
lazyjson::extractor ex("{\"key\": {\"key2\": {\"key3\": 123, \"key3a\": true, \"key3b\": \"hello\"}}, \"other_key\": 1.5}");

// cache the nested object at key2, using cached json as main
ex["key"]["key2"].cache();

// notice that you are within the cached object
int number = ex["key3"].extract().asInt();
bool boolean = ex["key3a"].extract().asBool();
std::string = ex["key3b"].extract().asString();

// reset to inital state
ex.reset();
float other = ex["other_key"].extract().asFloat(); // 1.5

```

The above example is faster than:

```cpp
// Create an extractor object
lazyjson::extractor ex("{\"key\": {\"key2\": {\"key3\": 123, \"key3a\": true, \"key3b\": \"hello\"}}}");

// this will cause iterating over the same json string many times!
// the cached example does exacly (1 + 1) + (1 + 2 + 3) = 8 key searches
// this example: (1 + 1 + 1) + (1 + 1 + 2) + (1 + 1 + 3) = 12
int number = ex["key"]["key2"]["key3"].extract().asInt();
bool boolean = ex["key"]["key2"]["key3a"].extract().asBool();
std::string = ex["key"]["key2"]["key3b"].extract().asString();


```

## Error Handling

The Lazy JSON Extraction Library is designed with robust error handling to ensure smooth operation even when dealing with malformed or invalid JSON data. 

### Usage

The library uses standard C++ exceptions, specifically `std::runtime_error`, to handle errors. This exception is thrown when an error occurs during the extraction process. 

```cpp
lazyjson::extractor ex("{\"key\": \"hello\", \"foo\": null}");

// Non-existing keys/indexes are supported, but only if the type is correct
// (non existing key - object, non existing index - list)
// ex[0].extract() will throw an exception
ex["abc"][0]["key"].extract().isNull() // true
// supports null propagation
ex["foo"]["abc"][0].extract().isNull() // true
try {
    // Potentially error-prone code goes here
    auto value = ex["key"].extract().as<int>(); // string cannot be converted to int
    auto value2 = ex[0].extract().as<bool>(); // this is an object, exception will be thrown
}
catch (const std::runtime_error& e) {
    // Handle the error

    auto data = ex["key"].extract();
    if (data.isNull()){
        // handle null
        // ...
    }
    if (data.type() == lazyjson::LazyType::STRING){
        // handle string
        // ...
    } 
}
```

In the above example, if a non-existing key or index is accessed on mismatching type, or if a value cannot be converted to the expected type, a `std::runtime_error` is thrown. The exception can be caught and handled in a catch block.

### Common Errors

Here are some common errors that can occur during the extraction process:

- **Type Mismatch**: This error occurs when you try to extract a value as a type that does not match the actual type in the JSON data. For example, trying to extract a string value as an integer or trying to access an object as an array would result in a type mismatch error.
- **Invalid Access**: This error occurs when you try to access a non-existing key or index. The library supports this operation, but only if the type is correct. For example, accessing a non-existing key in an object or a non-existing index in a list is allowed, but trying to access an index in an object or a key in a list would result in an invalid access error.

Each of these errors results in a `std::runtime_error` being thrown with a message that describes the error.

## Contributing

Contributions are welcome! Please submit a pull request or create an issue to contribute.

## License

This library is licensed under the MIT License. See the LICENSE file for more details.