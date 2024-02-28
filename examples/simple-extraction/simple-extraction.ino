/*
  Lazy JSON Extraction Library Example

  This example shows how to parse JSON data using the Lazy JSON Extraction Library.
  It parses a JSON object that contains data about someone 
  named "John" with an age of 30 and no car.

*/

// Include the Lazy JSON Extraction Library
#include <lazyjson.h>

void setup() {
    // Initialize the serial port to 115200 baud rate
    Serial.begin(115200);
}

void loop() {
    // Use the lazyjson namespace to avoid having to type lazyjson:: before each object
    using namespace lazyjson;

    String data("{\"name\":\"John\", \"age\":30, \"car\":null}");
    // name: "John"
    // age: 30
    // car: null

    // Initialize the extractor
    extractor ex(data.c_str());

    // Extract the values
    String name = ex["name"].extract().asString();
    int age = ex["age"].extract().as<int>();

    // Note that the car is null, so we need to check for it
    auto car = ex["car"].extract();

    // Print the results
    Serial.printf("name: %s\n", name.c_str());
    Serial.printf("age: %d\n", age);

    // Check if the car is null
    Serial.printf("car: %s\n", car.isNull() ? "null" : "not null");

    // Null propagation and not existing keys
    auto null_propagate = ex["car"]["model"][0].extract(); // extract from null
    auto not_existing = ex["not_existing"].extract(); // extract from non-existing key

    // Print the results
    Serial.printf("null_propagate: %s\n", null_propagate.isNull() ? "null" : "not null");
    Serial.printf("not_existing: %s\n", not_existing.isNull() ? "null" : "not null");
    
    // Also you can directly check if the value is null (without extracting it first)
    if (ex["car"].isNull()){
        Serial.println("car is null");
    }

    delay(5000);
}