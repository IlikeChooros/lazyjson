/*
  Lazy JSON Extraction Library Example

  This example shows how to parse JSON data using the Lazy JSON Extraction Library.
  It parses a JSON array that contains information about different locations named "Oslo" in various countries.

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

    // JSON data as a string
    String data(
    "["
        "{"
            "\"name\":\"Oslo\","
            "\"local_names\":"
                "{\"ca\":\"Oslo\",\"ky\":\"Осло\",\"hi\":\"ओस्लो\",\"ps\":\"اوسلو\",\"fr\":\"Oslo\",\"th\":\"ออสโล\",\"hy\":\"Օսլո\",\"hu\":\"Oslo\",\"el\":\"Όσλο\",\"bo\":\"ཨོ་སི་ལོ།\",\"cv\":\"Осло\",\"en\":\"Oslo\",\"ko\":\"오슬로\",\"es\":\"Oslo\",\"ga\":\"Osló\",\"co\":\"Oslu\",\"kk\":\"Осло\",\"os\":\"Осло\",\"oc\":\"Òslo\",\"my\":\"အော့စလိုမြို့\",\"bg\":\"Осло\",\"sv\":\"Oslo\",\"pl\":\"Oslo\",\"la\":\"Asloa\",\"am\":\"ኦስሎ\",\"lt\":\"Oslas\",\"ja\":\"オスロ\",\"tt\":\"Осло\",\"kv\":\"Осло\",\"fa\":\"اسلو\",\"ug\":\"ئوسلو\",\"ur\":\"اوسلو\",\"ml\":\"ഓസ്ലൊ\",\"ku\":\"Oslo\",\"de\":\"Oslo\",\"zh\":\"奧斯陸\",\"no\":\"Oslo\",\"tg\":\"Осло\",\"uk\":\"Осло\",\"ar\":\"أوسلو\",\"ru\":\"Осло\",\"be\":\"Осла\",\"mi\":\"Ōhoro\",\"pt\":\"Oslo\",\"is\":\"Ósló\",\"ka\":\"ოსლო\",\"mk\":\"Осло\",\"ne\":\"ओस्लो\",\"sr\":\"Осло\",\"mr\":\"ओस्लो\",\"ta\":\"ஒஸ்லோ\",\"he\":\"אוסלו\",\"eo\":\"Oslo\",\"it\":\"Oslo\",\"nn\":\"Oslo\"},"
            "\"lat\":59.9133301,"
            "\"lon\":10.7389701,"
            "\"country\":\"NO\""
        "},"
        "{"
            "\"name\":\"Oslo\","
            "\"lat\":48.1951323,"
            "\"lon\":-97.131159,"
            "\"country\":\"US\","
            "\"state\":\"Minnesota\""
        "},"
        "{"
            "\"name\":\"Oslo\","
            "\"local_names\":"
                "{\"ru\":\"Осло\"},"
            "\"lat\":59.97239745,"
            "\"lon\":10.775729194051895,"
            "\"country\":\"NO\""
        "},"
        "{"
            "\"name\":\"Oslo\","
            "\"lat\":43.8921855,"
            "\"lon\":-92.7385186,"
            "\"country\":\"US\","
            "\"state\":\"Minnesota\""
        "},"
        "{"
            "\"name\":\"Oslo\","
            "\"local_names\":"
                "{\"ug\":\"ئوسلو\",\"sr\":\"Осло\",\"en\":\"Oslo\",\"es\":\"Oslo\",\"gr\":\"Ὁλμία\",\"pa\":\"ਓਸਲੋ\",\"de\":\"Oslo\",\"ar\":\"أوسلو\",\"ur\":\"اوسلو\",\"be\":\"Осла\"},"
            "\"lat\":25.2289679,"
            "\"lon\":55.15787307408016,"
            "\"country\":\"AE\","
            "\"state\":\"Dubai\""
        "}"
    "]");

     // Create an extractor object with the JSON data
    extractor ex(data.c_str());

    // Initialize a counter for the loop
    int i = 0;

    // Loop through each object in the JSON array
    while(true){
        // If the current object is null, we've reached the end of the array
        if (ex[i].isNull()){
            break;
        }

        // Cache the current object for efficient extraction
        ex[i].cache();

        // Print the cached JSON object
        Serial.printf("\nParsing json: %s\n", ex.json().c_str());

        // Extract the name and country from the JSON object
        String name = ex["name"].extract().as<String>();
        String country = ex["country"].extract().as<String>();

        // Extract the state from the JSON object, or use "null" if it doesn't exist
        String state = ex["state"].extract().isNull() ? "null" : ex["state"].extract().as<String>();

        // Extract the latitude and longitude from the JSON object
        double lat = ex["lat"].extract().as<double>();
        double lon = ex["lon"].extract().as<double>();

        // Print the extracted values
        Serial.printf("Parsing object at index %d\n", i);
        Serial.println("Name: " + name);
        Serial.println("Country: " + country);
        Serial.println("State: " + state);
        Serial.println("Latitude: " + String(lat));
        Serial.println("Longitude: " + String(lon));

        // Reset the cache before moving on to the next object
        ex.reset();

        // Increment the counter
        i++;
    }

    // Wait for 5 seconds before the next loop iteration
    delay(5000);
}