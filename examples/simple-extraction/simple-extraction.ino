#include <lazyjson.h>

void setup() {
    // initialize the serial port
    Serial.begin(115200);
}

void loop() {
    using namespace lazyjson;

    String data("{\"name\":\"John\", \"age\":30, \"car\":null}");
    // name: "John"
    // age: 30
    // car: null

    // initialize the extractor
    extractor ex(data.c_str());

    // extract the values
    std::string name = ex["name"].extract().asString();
    int age = ex["age"].extract().as<int>();
    // note that the car is null, so we need to check for it
    auto car = ex["car"].extract();

    Serial.printf("name: %s\n", name.c_str());
    Serial.printf("age: %d\n", age);
    // check if the car is null
    Serial.printf("car: %s\n", car.isNull() ? "null" : "not null");

    // null propagation
    auto not_existing = ex["car"]["model"][0].extract(); // extract from null
    auto some_key = ex["some_key"].extract(); // not existing key

    Serial.printf("not_existing: %s\n", not_existing.isNull() ? "null" : "not null");
    Serial.printf("some_key: %s\n", some_key.isNull() ? "null" : "not null");

    delay(5000);
}