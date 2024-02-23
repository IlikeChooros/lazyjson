# Lazy JSON Extraction Library

This library provides a simple and efficient way to extract JSON data in C++. It's designed to be lightweight and easy to use, making it ideal for resource-constrained environments.

## Features

- **Lazy Extraction**: Extract only the data you need, when you need it. This can significantly reduce memory usage and improve performance when working with large JSON documents.
- **Easy to Use**: The library provides a simple and intuitive API. You can extract data using the `[]` operator and string keys or integer indices.
- **Error Handling**: The library includes robust error handling to help you catch and handle errors in your JSON data.

## How to Use

### Include the Library

Include the [`lazyjson.h`](command:_github.copilot.openRelativePath?%5B%22lazyjson.h%22%5D "lazyjson.h") file in your project.

```cpp
#include "lazyjson.h"
```

### Create an Extractor

Create an `extractor` object with your JSON data.

```cpp
extractor ex(json_data);
```

### Extract Data

You can extract data using the `[]` operator. Use a string key to extract a value from an object, or an integer index to extract a value from an array.

```cpp
std::string value = ex["key"].extract().asString();
int value = ex[0].extract().asInt();
```

You can also chain `[]` operators to extract nested data.

```cpp
std::string value = ex["key"]["nested_key"].extract().asString();
```

### Reset the Extractor

You can reset the extractor to its initial state using the `reset` method.

```cpp
ex.reset();
```

### Caching

The `extractor` class doesn't allow value caching, but it can cache an whole object or list (as a string).
To improve performance you can use this caching technique when extracting data from the same object or list.
The effects drastically magnify when working with bigger structures.


Here's an example of how to use the `cache()` method:

```cpp
// Create an extractor object
lazyjson::extractor ex("{\"key\": {\"key2\": {\"key3\": 123, \"key3a\": true, \"key3b\": \"hello\"}}}");

// cache the nested object at key2
ex["key"]["key2"].cache();

// get the cached json
std::string cached = ex.json();

// load cached string, update the json
ex.use(cached.c_str());

int number = ex["key3"].extract().asInt();
bool boolean = ex["key3a"].extract().asBool();
std::string = ex["key3b"].extract().asString();

```

The above example is faster than:

```cpp
// Create an extractor object
lazyjson::extractor ex("{\"key\": {\"key2\": {\"key3\": 123, \"key3a\": true, \"key3b\": \"hello\"}}}");

// this will couse iterating over the json string more times as opposed to caching example!
// the cached example does exacly (1 + 1) + (1 + 2 + 3) = 8 key searchings
// this example: (1 + 1 + 1) + (1 + 1 + 2) + (1 + 1 + 3) = 12
int number = ex["key"]["key2"]["key3"].extract().asInt();
bool boolean = ex["key"]["key2"]["key3a"].extract().asBool();
std::string = ex["key"]["key2"]["key3b"].extract().asString();


```

## Contributing

Contributions are welcome! Please submit a pull request or create an issue to contribute.

## License

This library is licensed under the MIT License. See the LICENSE file for more details.