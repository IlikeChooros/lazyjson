
# Lazy JSON Extraction Library

This library provides a simple and efficient way to extract JSON data in C++. It's designed to be lightweight and easy to use, making it ideal for resource-constrained environments such as Arduino.

## Features

- **Lazy Extraction**: Extract only the data you need, when you need it. This can significantly reduce memory usage and improve performance when working with large JSON documents.
- **Easy to Use**: The library provides a simple and intuitive API. You can extract data using the `[]` operator and string keys or integer indices.
- **Error Handling**: The library includes robust error handling to help you catch and handle errors in your JSON data.

## Usage

### Include the Library

Include the `lazyjson.h` file in your project.

```cpp
#include <lazyjson.h>
```

### Create an Extractor

Create an `extractor` object with your JSON data.

```cpp
lazyjson::extractor ex("{\"version\": [\"lazyjson\", 1.0], \"key\": true}");
```

### Extract Data

You can extract data using the `[]` operator. Use a string key to extract a value from an object, or an integer index to extract a value from an array.

```cpp
bool value = ex["key"].extract().asBool(); // true
double version = ex["version"][1].extract().asDouble(); // 1.0
```

You can also chain `[]` operators to extract nested data.

```cpp
std::string value = ex["key"]["nested_key"].extract().asString();
```

### Caching

The `extractor` class allows caching current value (as a json string). This can improve performance when extracting data from the same object or list, especially with larger structures.

Here's an example of how to use the `cache()` method:

```cpp
lazyjson::extractor ex("{\"key\": {\"key2\": {\"key3\": 123, \"key3a\": true, \"key3b\": \"hello\"}}, \"other_key\": 1.5}");

ex["key"]["key2"].cache();

int number = ex["key3"].extract().asInt(); // 123
bool boolean = ex["key3a"].extract().asBool(); // true
std::string text = ex["key3b"].extract().asString(); // "hello"

ex.reset();
float other = ex["other_key"].extract().asFloat(); // 1.5
```

### Error Handling

The library uses standard C++ exceptions, specifically `std::runtime_error`, to handle errors. This exception is thrown when an error occurs during the extraction process. 

```cpp
lazyjson::extractor ex("{\"key\": \"hello\", \"foo\": null}");

try {
    auto value = ex["key"].extract().as<int>(); // string cannot be converted to int
    auto value2 = ex[0].extract().as<bool>(); // this is an object, exception will be thrown
}
catch (const std::runtime_error& e) {
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

Common errors include type mismatch and invalid access. Each of these errors results in a `std::runtime_error` being thrown with a message that describes the error.

## Known Issues

There is a slight memory leak when an exception is thrown.

## Contributing

Contributions are welcome! Please submit a pull request or create an issue to contribute.

## License

This library is licensed under the MIT License. See the LICENSE file for more details.

