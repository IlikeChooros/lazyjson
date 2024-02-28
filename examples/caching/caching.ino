/*
  Lazy JSON Extraction Library Example

  This example shows how to parse JSON data using the Lazy JSON Extraction Library.
  It parses a JSON object with example nested data, showing the usage of caching.
  
*/

// Include the Lazy JSON Extraction Library
#include <lazyjson.h>

void setup() {
    // initialize the serial port
    Serial.begin(115200);
}

void loop() {
    // Use the lazyjson namespace to avoid having to type lazyjson:: before each object
    using namespace lazyjson;

    // JSON data as a string
    String data("{\"key\": {\"nested\": {\"value\": 42, \"array\": [1, true, \"string\"], \"object\": {\"key\": \"value\"}}}, \"key2\": 42}");
    // key:
    //   nested:
    //     value: 42
    //     array:
    //       - 1
    //       - true
    //       - "string"
    //     object:
    //       key: "value"
    // key2: 42

    // Initialize the extractor
    extractor ex(data.c_str());

    // Cache the nested object
    ex["key"]["nested"].cache();

    // Extract the values, notice that we don't need to specify the full path,
    // but only the keys of the nested object
    int value = ex["value"].extract().as<int>();
    bool boolean = ex["array"][1].extract().as<bool>();
    String string = ex["object"]["key"].extract().as<String>();

    Serial.printf("value: %d\n", value);
    Serial.printf("boolean: %d\n", boolean);
    Serial.printf("string: %s\n", string.c_str());

    // Reset the cache, set data to the initial state
    ex.reset();

    // Get key2 from the root object
    int key2 = ex["key2"].extract().as<int>();

    Serial.printf("key2: %d\n", key2);

    delay(5000);
}