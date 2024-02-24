#include <lazyjson.h>

void setup() {
    // initialize the serial port
    Serial.begin(115200);
}

void loop() {
    delay(1000);

    using namespace lazyjson;

    // // debug heap usage only on ESP32
    // auto heapBefore = ESP.getFreeHeap();

    
    // nested object
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

    // load the data
    extractor ex(data.c_str());

    // cache the nested object
    ex["key"]["nested"].cache();

    // extract the values, notice that we don't need to specify the full path,
    // but only the keys of the nested object
    int value = ex["value"].extract().as<int>();
    bool boolean = ex["array"][1].extract().as<bool>();
    std::string string = ex["object"]["key"].extract().asString();

    Serial.printf("value: %d\n", value);
    Serial.printf("boolean: %d\n", boolean);
    Serial.printf("string: %s\n", string.c_str());

    // reset the cache, set data to the initial state
    ex.reset();

    // get key2 from the root object
    int key2 = ex["key2"].extract().as<int>();

    Serial.printf("key2: %d\n", key2);

    // auto heapAfter = ESP.getFreeHeap();
    // Serial.printf("Heap before: %d, heap after: %d, diff: %d\n", heapBefore, heapAfter, heapBefore - heapAfter);
    delay(5000);
}