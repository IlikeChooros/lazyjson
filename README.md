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


### Caching

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

## Contributing

Contributions are welcome! Please submit a pull request or create an issue to contribute.

## License

This library is licensed under the MIT License. See the LICENSE file for more details.