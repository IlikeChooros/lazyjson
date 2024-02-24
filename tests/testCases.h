#pragma once

#include "TestCase.h"

#include <lazyjson.h>

#include <vector>

using namespace lazyjson;

namespace tests
{
    struct MemoryWatchpoint
    {
        size_t startMem = 0;
        std::string name;
        size_t endMem = 0;
    };

    class JsonTestCase : public TestCase
    {
    public:
        JsonTestCase(std::string name = "JsonTestCase") : TestCase(name) {}
        virtual void test() {}

    protected:
        void assertLazyType(const lazyjson::LazyTypedValues &node, const lazyjson::LazyType &type);

        template <class T>
        void assertEqual(const T &o1, const T &o2, char *format = "")
        {
            try
            {
                _assert(o1 == o2, "assertEqual");
            }
            catch (const std::runtime_error &e)
            {
                if (format != "")
                {
                    Serial.printf(format, o1, o2);
                }
                throw e;
            }
        }

        void setMemoryWatchpoint(const char *watchpoint = "%testname");
        virtual void _report(long long timeNs, long long timeMs, size_t memoryDiff);

        std::vector<MemoryWatchpoint> _memoryWatchpoints;
    };

// extractor tests

    class LazyExtractorNumberTest : public JsonTestCase{
    public:
        LazyExtractorNumberTest() : JsonTestCase("LazyExtractorNumberTest") {}

        void test(){
            setMemoryWatchpoint();
            extractor extractor("-3.141592");
            assertLazyType(extractor.extract().raw(), LazyType::NUMBER);
            setMemoryWatchpoint();
        }
    };

    class LazyExtractorBoolTest : public JsonTestCase{
    public:
        LazyExtractorBoolTest() : JsonTestCase("LazyExtractorBoolTest") {}

        void test(){
            setMemoryWatchpoint();
            extractor extractor("true");
            assertLazyType(extractor.extract().raw(), LazyType::BOOL);
            assertEqual(extractor.extract().asBool(), true, " %i != %i");
            setMemoryWatchpoint();
        }
    };

    class LazyExtractorStringTest : public JsonTestCase{
    public:
        LazyExtractorStringTest() : JsonTestCase("LazyExtractorStringTest") {}

        void test(){
            setMemoryWatchpoint();
            extractor extractor("\"mystring\"");
            assertLazyType(extractor.extract().raw(), LazyType::STRING);
            assertEqual<std::string>(extractor.extract().asString(), "mystring");
            setMemoryWatchpoint();
        }
    };

    class LazyExtractorObjectTest: public JsonTestCase{
    public:
        LazyExtractorObjectTest() : JsonTestCase("LazyExtractorObjectTest") {}

        void test(){
            setMemoryWatchpoint();
            extractor extractor("{\"foo\": [], \"key\": \"string\"}");

            assertLazyType(extractor["foo"].extract().raw(), LazyType::LIST);
            assertLazyType(extractor["key"].extract().raw(), LazyType::STRING);
            assertEqual<std::string>(extractor["key"].extract().asString(), "string");

            setMemoryWatchpoint();
        }
    };

    class LazyExtractorObjectWithNumbersTest: public JsonTestCase{
    public:
        LazyExtractorObjectWithNumbersTest() : JsonTestCase("LazyExtractorObjectWithNumbersTest") {}

        void test(){
            setMemoryWatchpoint();
            extractor extractor("{\"foo\": -0.54, \"pi\": 3.14159265, \"one\": 1}");

            assertLazyType(extractor["foo"].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor["pi"].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor["one"].extract().raw(), LazyType::NUMBER);
            assertEqual(extractor["one"].extract().asFloat(), 1.0f, " %f != %f");
            setMemoryWatchpoint();
        }
    };

    class LazyExtractorListWithNumbersTest: public JsonTestCase{
    public:
        LazyExtractorListWithNumbersTest() : JsonTestCase("LazyExtractorListWithNumbersTest") {}

        void test(){
            setMemoryWatchpoint();
            extractor extractor("[-0.54, 3.14159265, 1]");

            assertLazyType(extractor[0].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor[1].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor[2].extract().raw(), LazyType::NUMBER);
            assertEqual(extractor[2].extract().asFloat(), 1.0f, " %f != %f");
            setMemoryWatchpoint();
        }
    };

    class LazyExtractorComplexApiWeatherData: public JsonTestCase{
    public:
        LazyExtractorComplexApiWeatherData() : JsonTestCase("LazyExtractorComplexApiWeatherData") {}

        void test(){
            setMemoryWatchpoint();
            extractor extractor(
                "{\"coord\":{\"lon\":17.2903,\"lat\":50.9571},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"base\":\"stations\",\"main\":"
                "{\"temp\":-6.26,\"feels_like\":-12.88,\"temp_min\":-7.22,\"temp_max\":-6.03,\"pressure\":1020,\"humidity\":77,\"sea_level\":1020,\"grnd_level\":1004},\"visibility\":10000,"
                "\"wind\":{\"speed\":5.2,\"deg\":17,\"gust\":8.05},\"clouds\":{\"all\":100},\"dt\":1704642926,\"sys\":{\"type\":2,\"id\":2034837,\"country\":\"PL\",\"sunrise\":1704610351,\"sunset\":1704639641},\"timezone\":3600,\"id\":7532481,\"name\":\"Oława\",\"cod\":200}");

            assertLazyType(extractor["coord"]["lon"].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor["coord"]["lat"].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor["weather"][0]["id"].extract().raw(), LazyType::NUMBER);

            assertLazyType(extractor["name"].extract().raw(), LazyType::STRING);
            assertLazyType(extractor["main"]["temp"].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor["main"]["feels_like"].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor["visibility"].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor["sys"]["sunrise"].extract().raw(), LazyType::NUMBER);

            extractor["main"].cache();
            assertEqual(extractor["humidity"].extract().asFloat(),77.0f, " %f != %f");
            setMemoryWatchpoint();
        }
    };

    class LazyExtractorExampleTest: public JsonTestCase{
    public:
        LazyExtractorExampleTest() : JsonTestCase("LazyExtractorExampleTest") {}

        void test(){
            setMemoryWatchpoint();
            extractor extractor(
                "{\"foo\": [1,\"str\",true, null, {\"key\": {}}],"
                "\"key\": \"string value\","
                "\"object\": "
                "{"
                "\"list\": [1,3,5],"
                "\"bool\": true,"
                "\"null\": null"
                "}"
                "}");

            assertLazyType(extractor["foo"][0].extract().raw(), LazyType::NUMBER);
            assertLazyType(extractor["key"].extract().raw(), LazyType::STRING);

            extractor["object"].cache();

            assertEqual(extractor["list"][0].extract().asInt(), 1, " %i != %i \n");
            assertEqual(extractor["bool"].extract().asBool(), true, " %i != %i \n");
            assertEqual(extractor["null"].extract().asNull(), true, " %i != %i \n");

            extractor["list"].cache();

            assertEqual(extractor[0].extract().asInt(), 1, " %i != %i \n");
            assertEqual(extractor[1].extract().asInt(), 3, " %i != %i \n");
            assertEqual(extractor[2].extract().asInt(), 5, " %i != %i \n");
            
            setMemoryWatchpoint();
        }
    };

    // coord
        //     lon	17.2903
        //     lat	50.9571
        // weather
        //     0
        //         id	804
        //         main	"Clouds"
        //         description	"zachmurzenie duże"
        //         icon	"04n"
        // base	"stations"
        // main
        //     temp	-6.26
        //     feels_like	-12.88
        //     temp_min	-7.22
        //     temp_max	-6.03
        //     pressure	1020
        //     humidity	77
        //     sea_level	1020
        //     grnd_level	1004
        // visibility	10000
        // wind
        //     speed	5.2
        //     deg	17
        //     gust	8.05
        // clouds
        //     all	100
        //     dt	1704642926
        // sys
        //     type	2
        //     id	2034837
        //     country	"PL"
        //     sunrise	1704610351
        //     sunset	1704639641
        // timezone	3600
        // id	7532481
        // name	"Oława"
        // cod	200

      class LazyExtractorForecastApiData : public JsonTestCase
    {
    public:
        LazyExtractorForecastApiData() : JsonTestCase("LazyExtractorForecastApiData") {}

        void test()
        {
            setMemoryWatchpoint();
            // Not gonna visualize this nicely -.-

            const char* data = "{\"cod\":\"200\",\"message\":0,\"cnt\":40,\"list\":[{\"dt\":1704650400,\"main\":{\"temp\":-6.7,\"feels_like\":-13.23,\"temp_min\":-6.7,\"temp_max\":-5.91,\"pressure\":1021,\"sea_level\":1021,\"grnd_level\":1005,\"humidity\":78,\"temp_kf\":-0.79},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":4.91,\"deg\":16,\"gust\":7.55},\"visibility\":10000,\"pop\":0.16,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-07 18:00:00\"},{\"dt\":1704661200,\"main\":{\"temp\":-6.71,\"feels_like\":-12.99,\"temp_min\":-6.72,\"temp_max\":-6.71,\"pressure\":1022,\"sea_level\":1022,\"grnd_level\":1007,\"humidity\":79,\"temp_kf\":0.01},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":4.57,\"deg\":12,\"gust\":6.99},\"visibility\":10000,\"pop\":0.12,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-07 21:00:00\"},{\"dt\":1704672000,\"main\":{\"temp\":-7.21,\"feels_like\":-13.51,\"temp_min\":-7.46,\"temp_max\":-7.21,\"pressure\":1023,\"sea_level\":1023,\"grnd_level\":1008,\"humidity\":79,\"temp_kf\":0.25},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":98},\"wind\":{\"speed\":4.44,\"deg\":14,\"gust\":6.86},\"visibility\":10000,\"pop\":0.12,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 00:00:00\"},{\"dt\":1704682800,\"main\":{\"temp\":-7.55,\"feels_like\":-13.91,\"temp_min\":-7.55,\"temp_max\":-7.55,\"pressure\":1025,\"sea_level\":1025,\"grnd_level\":1009,\"humidity\":79,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":4.42,\"deg\":17,\"gust\":6.91},\"visibility\":10000,\"pop\":0.03,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 03:00:00\"},{\"dt\":1704693600,\"main\":{\"temp\":-7.86,\"feels_like\":-14.1,\"temp_min\":-7.86,\"temp_max\":-7.86,\"pressure\":1027,\"sea_level\":1027,\"grnd_level\":1011,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":4.18,\"deg\":20,\"gust\":7.12},\"visibility\":10000,\"pop\":0.03,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 06:00:00\"},{\"dt\":1704704400,\"main\":{\"temp\":-7.25,\"feels_like\":-13.87,\"temp_min\":-7.25,\"temp_max\":-7.25,\"pressure\":1029,\"sea_level\":1029,\"grnd_level\":1012,\"humidity\":74,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04d\"}],\"clouds\":{\"all\":86},\"wind\":{\"speed\":4.84,\"deg\":34,\"gust\":7.22},\"visibility\":10000,\"pop\":0.02,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-08 09:00:00\"},{\"dt\":1704715200,\"main\":{\"temp\":-5.88,\"feels_like\":-12.21,\"temp_min\":-5.88,\"temp_max\":-5.88,\"pressure\":1029,\"sea_level\":1029,\"grnd_level\":1013,\"humidity\":64,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"zachmurzenie umiarkowane\",\"icon\":\"04d\"}],\"clouds\":{\"all\":75},\"wind\":{\"speed\":4.92,\"deg\":39,\"gust\":6.81},\"visibility\":10000,\"pop\":0.01,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-08 12:00:00\"},{\"dt\":1704726000,\"main\":{\"temp\":-6.97,\"feels_like\":-12.87,\"temp_min\":-6.97,\"temp_max\":-6.97,\"pressure\":1031,\"sea_level\":1031,\"grnd_level\":1015,\"humidity\":74,\"temp_kf\":0},\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"zachmurzenie małe\",\"icon\":\"03d\"}],\"clouds\":{\"all\":28},\"wind\":{\"speed\":4.03,\"deg\":27,\"gust\":7.24},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-08 15:00:00\"},{\"dt\":1704736800,\"main\":{\"temp\":-7.82,\"feels_like\":-13.27,\"temp_min\":-7.82,\"temp_max\":-7.82,\"pressure\":1033,\"sea_level\":1033,\"grnd_level\":1016,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":801,\"main\":\"Clouds\",\"description\":\"pochmurnie\",\"icon\":\"02n\"}],\"clouds\":{\"all\":21},\"wind\":{\"speed\":3.35,\"deg\":34,\"gust\":6.81},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 18:00:00\"},{\"dt\":1704747600,\"main\":{\"temp\":-8.45,\"feels_like\":-13.06,\"temp_min\":-8.45,\"temp_max\":-8.45,\"pressure\":1034,\"sea_level\":1034,\"grnd_level\":1018,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":2.52,\"deg\":35,\"gust\":5.14},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 21:00:00\"},{\"dt\":1704758400,\"main\":{\"temp\":-9.22,\"feels_like\":-13.7,\"temp_min\":-9.22,\"temp_max\":-9.22,\"pressure\":1035,\"sea_level\":1035,\"grnd_level\":1019,\"humidity\":83,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":2.33,\"deg\":30,\"gust\":4.98},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 00:00:00\"},{\"dt\":1704769200,\"main\":{\"temp\":-9.76,\"feels_like\":-13.35,\"temp_min\":-9.76,\"temp_max\":-9.76,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":85,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":4},\"wind\":{\"speed\":1.73,\"deg\":37,\"gust\":3.05},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 03:00:00\"},{\"dt\":1704780000,\"main\":{\"temp\":-10.14,\"feels_like\":-10.14,\"temp_min\":-10.14,\"temp_max\":-10.14,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":4},\"wind\":{\"speed\":1.07,\"deg\":78,\"gust\":1.34},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 06:00:00\"},{\"dt\":1704790800,\"main\":{\"temp\":-7.59,\"feels_like\":-10.53,\"temp_min\":-7.59,\"temp_max\":-7.59,\"pressure\":1038,\"sea_level\":1038,\"grnd_level\":1021,\"humidity\":75,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":4},\"wind\":{\"speed\":1.56,\"deg\":125,\"gust\":2.78},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-09 09:00:00\"},{\"dt\":1704801600,\"main\":{\"temp\":-5.29,\"feels_like\":-7.53,\"temp_min\":-5.29,\"temp_max\":-5.29,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":65,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":3},\"wind\":{\"speed\":1.37,\"deg\":109,\"gust\":2.58},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-09 12:00:00\"},{\"dt\":1704812400,\"main\":{\"temp\":-7.38,\"feels_like\":-7.38,\"temp_min\":-7.38,\"temp_max\":-7.38,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":82,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":2},\"wind\":{\"speed\":0.9,\"deg\":250,\"gust\":1.02},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-09 15:00:00\"},{\"dt\":1704823200,\"main\":{\"temp\":-8.82,\"feels_like\":-8.82,\"temp_min\":-8.82,\"temp_max\":-8.82,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":89,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":3},\"wind\":{\"speed\":1.28,\"deg\":241,\"gust\":1.24},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 18:00:00\"},{\"dt\":1704834000,\"main\":{\"temp\":-9.37,\"feels_like\":-9.37,\"temp_min\":-9.37,\"temp_max\":-9.37,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":90,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":1.07,\"deg\":202,\"gust\":1.02},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 21:00:00\"},{\"dt\":1704844800,\"main\":{\"temp\":-9.53,\"feels_like\":-12.45,\"temp_min\":-9.53,\"temp_max\":-9.53,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":88,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":1.42,\"deg\":188,\"gust\":1.36},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 00:00:00\"},{\"dt\":1704855600,\"main\":{\"temp\":-9.8,\"feels_like\":-13.52,\"temp_min\":-9.8,\"temp_max\":-9.8,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":86,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":1.8,\"deg\":173,\"gust\":1.78},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 03:00:00\"},{\"dt\":1704866400,\"main\":{\"temp\":-9.88,\"feels_like\":-13.7,\"temp_min\":-9.88,\"temp_max\":-9.88,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":83,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":1.85,\"deg\":167,\"gust\":1.81},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 06:00:00\"},{\"dt\":1704877200,\"main\":{\"temp\":-7.14,\"feels_like\":-11.04,\"temp_min\":-7.14,\"temp_max\":-7.14,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":69,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":2},\"wind\":{\"speed\":2.18,\"deg\":176,\"gust\":3.03},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-10 09:00:00\"},{\"dt\":1704888000,\"main\":{\"temp\":-3.91,\"feels_like\":-6.07,\"temp_min\":-3.91,\"temp_max\":-3.91,\"pressure\":1035,\"sea_level\":1035,\"grnd_level\":1019,\"humidity\":60,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":1},\"wind\":{\"speed\":1.42,\"deg\":151,\"gust\":1.62},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-10 12:00:00\"},{\"dt\":1704898800,\"main\":{\"temp\":-6.31,\"feels_like\":-9.39,\"temp_min\":-6.31,\"temp_max\":-6.31,\"pressure\":1034,\"sea_level\":1034,\"grnd_level\":1017,\"humidity\":80,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":0},\"wind\":{\"speed\":1.74,\"deg\":182,\"gust\":1.69},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-10 15:00:00\"},{\"dt\":1704909600,\"main\":{\"temp\":-7.5,\"feels_like\":-9.97,\"temp_min\":-7.5,\"temp_max\":-7.5,\"pressure\":1033,\"sea_level\":1033,\"grnd_level\":1017,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":2},\"wind\":{\"speed\":1.34,\"deg\":185,\"gust\":1.32},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 18:00:00\"},{\"dt\":1704920400,\"main\":{\"temp\":-7.79,\"feels_like\":-7.79,\"temp_min\":-7.79,\"temp_max\":-7.79,\"pressure\":1032,\"sea_level\":1032,\"grnd_level\":1016,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":7},\"wind\":{\"speed\":0.85,\"deg\":197,\"gust\":0.82},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 21:00:00\"},{\"dt\":1704931200,\"main\":{\"temp\":-7.81,\"feels_like\":-10.66,\"temp_min\":-7.81,\"temp_max\":-7.81,\"pressure\":1031,\"sea_level\":1031,\"grnd_level\":1015,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":10},\"wind\":{\"speed\":1.5,\"deg\":244,\"gust\":1.44},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 00:00:00\"},{\"dt\":1704942000,\"main\":{\"temp\":-6.82,\"feels_like\":-10.65,\"temp_min\":-6.82,\"temp_max\":-6.82,\"pressure\":1029,\"sea_level\":1029,\"grnd_level\":1013,\"humidity\":79,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":2.17,\"deg\":251,\"gust\":3.15},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 03:00:00\"},{\"dt\":1704952800,\"main\":{\"temp\":-5.95,\"feels_like\":-10.63,\"temp_min\":-5.95,\"temp_max\":-5.95,\"pressure\":1028,\"sea_level\":1028,\"grnd_level\":1012,\"humidity\":74,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":7},\"wind\":{\"speed\":2.98,\"deg\":270,\"gust\":7.31},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 06:00:00\"},{\"dt\":1704963600,\"main\":{\"temp\":-2.82,\"feels_like\":-8.21,\"temp_min\":-2.82,\"temp_max\":-2.82,\"pressure\":1026,\"sea_level\":1026,\"grnd_level\":1010,\"humidity\":73,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"zachmurzenie umiarkowane\",\"icon\":\"04d\"}],\"clouds\":{\"all\":71},\"wind\":{\"speed\":4.65,\"deg\":278,\"gust\":10.91},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-11 09:00:00\"},{\"dt\":1704974400,\"main\":{\"temp\":-0.01,\"feels_like\":-5.21,\"temp_min\":-0.01,\"temp_max\":-0.01,\"pressure\":1025,\"sea_level\":1025,\"grnd_level\":1009,\"humidity\":78,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04d\"}],\"clouds\":{\"all\":85},\"wind\":{\"speed\":5.51,\"deg\":290,\"gust\":11.31},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-11 12:00:00\"},{\"dt\":1704985200,\"main\":{\"temp\":-1.03,\"feels_like\":-6.41,\"temp_min\":-1.03,\"temp_max\":-1.03,\"pressure\":1024,\"sea_level\":1024,\"grnd_level\":1009,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"zachmurzenie umiarkowane\",\"icon\":\"04d\"}],\"clouds\":{\"all\":76},\"wind\":{\"speed\":5.36,\"deg\":306,\"gust\":10.72},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-11 15:00:00\"},{\"dt\":1704996000,\"main\":{\"temp\":-0.82,\"feels_like\":-6.1,\"temp_min\":-0.82,\"temp_max\":-0.82,\"pressure\":1025,\"sea_level\":1025,\"grnd_level\":1009,\"humidity\":96,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":85},\"wind\":{\"speed\":5.27,\"deg\":309,\"gust\":10.83},\"visibility\":461,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 18:00:00\"},{\"dt\":1705006800,\"main\":{\"temp\":-0.4,\"feels_like\":-5.16,\"temp_min\":-0.4,\"temp_max\":-0.4,\"pressure\":1026,\"sea_level\":1026,\"grnd_level\":1011,\"humidity\":96,\"temp_kf\":0},\"weather\":[{\"id\":600,\"main\":\"Snow\",\"description\":\"słabe opady śniegu\",\"icon\":\"13n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":4.58,\"deg\":331,\"gust\":9.62},\"visibility\":1894,\"pop\":0.59,\"snow\":{\"3h\":0.34},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 21:00:00\"},{\"dt\":1705017600,\"main\":{\"temp\":-0.58,\"feels_like\":-5.21,\"temp_min\":-0.58,\"temp_max\":-0.58,\"pressure\":1027,\"sea_level\":1027,\"grnd_level\":1012,\"humidity\":88,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":4.31,\"deg\":340,\"gust\":9.01},\"visibility\":10000,\"pop\":0.43,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-12 00:00:00\"},{\"dt\":1705028400,\"main\":{\"temp\":-1.32,\"feels_like\":-6.01,\"temp_min\":-1.32,\"temp_max\":-1.32,\"pressure\":1029,\"sea_level\":1029,\"grnd_level\":1013,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":4.14,\"deg\":339,\"gust\":8.63},\"visibility\":10000,\"pop\":0.1,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-12 03:00:00\"},{\"dt\":1705039200,\"main\":{\"temp\":-3.83,\"feels_like\":-7.52,\"temp_min\":-3.83,\"temp_max\":-3.83,\"pressure\":1030,\"sea_level\":1030,\"grnd_level\":1014,\"humidity\":95,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"zachmurzenie umiarkowane\",\"icon\":\"04n\"}],\"clouds\":{\"all\":76},\"wind\":{\"speed\":2.47,\"deg\":347,\"gust\":5.07},\"visibility\":10000,\"pop\":0.1,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-12 06:00:00\"},{\"dt\":1705050000,\"main\":{\"temp\":-2.26,\"feels_like\":-5.62,\"temp_min\":-2.26,\"temp_max\":-2.26,\"pressure\":1032,\"sea_level\":1032,\"grnd_level\":1016,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":801,\"main\":\"Clouds\",\"description\":\"pochmurnie\",\"icon\":\"02d\"}],\"clouds\":{\"all\":11},\"wind\":{\"speed\":2.43,\"deg\":352,\"gust\":4.21},\"visibility\":10000,\"pop\":0.02,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-12 09:00:00\"},{\"dt\":1705060800,\"main\":{\"temp\":-0.94,\"feels_like\":-3.96,\"temp_min\":-0.94,\"temp_max\":-0.94,\"pressure\":1032,\"sea_level\":1032,\"grnd_level\":1016,\"humidity\":71,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":9},\"wind\":{\"speed\":2.34,\"deg\":325,\"gust\":3.48},\"visibility\":10000,\"pop\":0.01,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-12 12:00:00\"},{\"dt\":1705071600,\"main\":{\"temp\":-3.62,\"feels_like\":-5.68,\"temp_min\":-3.62,\"temp_max\":-3.62,\"pressure\":1032,\"sea_level\":1032,\"grnd_level\":1016,\"humidity\":86,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":7},\"wind\":{\"speed\":1.39,\"deg\":288,\"gust\":1.52},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-12 15:00:00\"}],\"city\":{\"id\":7532481,\"name\":\"Oława\",\"coord\":{\"lat\":50.9571,\"lon\":17.2903},\"country\":\"PL\",\"population\":0,\"timezone\":3600,\"sunrise\":1704610351,\"sunset\":1704639641}}";

            extractor ex(data);
            
            for (int i = 0; i < 40; i++){

                ex.reset();
                ex["list"][i].cache();

                assertLazyType(ex["main"]["feels_like"].extract().raw(), LazyType::NUMBER);
                assertLazyType(ex["main"]["temp"].extract().raw(), LazyType::NUMBER);
                assertLazyType(ex["main"]["pressure"].extract().raw(), LazyType::NUMBER);
                assertLazyType(ex["main"]["humidity"].extract().raw(), LazyType::NUMBER);

                assertLazyType(ex["weather"][0]["icon"].extract().raw(), LazyType::STRING);
            }

            setMemoryWatchpoint();
        }
    };
/*


    // complex tests
    class ComplexObjectParse : public JsonTestCase
    {
    public:
        ComplexObjectParse() : JsonTestCase("ComplexObjectParse") {}

        void test()
        {
            setMemoryWatchpoint();
            JsonParser parser(
                "{"
                "\"foo\": [1,\"str\",true, null, {\"key\": {}}],"
                "\"key\": \"string value\","
                "\"object\": "
                "{"
                "\"list\": [1,3,5],"
                "\"bool\": true,"
                "\"null\": null"
                "}"
                "}");

            auto data = parser.parse()->json();

            assertType(*data->returnObject()->at("foo"), Type::list);
            assertType(*data->returnObject()->at("key"), Type::string);
            assertType(*data->returnObject()->at("object"), Type::object);
            assertType(*data->returnObject()->at("object")->returnObject()->at("list"), Type::list);
            assertType(*data->returnObject()->at("object")->returnObject()->at("bool"), Type::boolean);
            assertType(*data->returnObject()->at("object")->returnObject()->at("null"), Type::null);
            setMemoryWatchpoint();
        }
    };

    class ComplexRealDataParse : public JsonTestCase
    {
    public:
        ComplexRealDataParse() : JsonTestCase("ComplexRealDataParse") {}

        void test()
        {
            setMemoryWatchpoint();
            JsonParser parser(
                "{"
                "\"table\":"
                "[ "
                "{\"id\":33,\"name\":\"Stowarzyszenie Szczot Sp.j.\",\"deleted\":false},"
                "{\"id\":32,\"name\":\"Gabinety Waloch-Bazyluk s.c.\",\"deleted\":false},"
                "{\"id\":31,\"name\":\"Kijas-Bąkiewicz i syn s.c.\",\"deleted\":false},"
                "{\"id\":30,\"name\":\"Stowarzyszenie Ciuba\",\"deleted\":false},"
                "{\"id\":29,\"name\":\"Stanuch-Rubin i syn s.c.\",\"deleted\":false},"
                "{\"id\":28,\"name\":\"Toczko Sp.j.\",\"deleted\":false},"
                "{\"id\":27,\"name\":\"Fundacja Steckiewicz-Juszczuk Sp.k.\",\"deleted\":false},"
                "{\"id\":26,\"name\":\"Gabinety Horbacz Sp.j.\",\"deleted\":false},"
                "{\"id\":25,\"name\":\"Fundacja Stempin Sp. z o.o. Sp.k.\",\"deleted\":false},"
                "{\"id\":24,\"name\":\"PPUH Romejko-Sabak Sp.k.\",\"deleted\":false}"
                "],"
                "\"size\":83"
                "}");
            auto data = parser.parse()->json();

            assertType(*data, Type::object);
            assertType((*data)["table"], Type::list);
            assertType((*data)["size"], Type::number);
            setMemoryWatchpoint();
        }
    };


  

    // Edge cases

    class EdgeAssignKeyToListInFilter : public JsonTestCase
    {
    public:
        EdgeAssignKeyToListInFilter() : JsonTestCase("EdgeAssignKeyToListInFilter") {}

        void test()
        {
            setMemoryWatchpoint();
            JsonParser parser("[1, {\"foo\": 123.5}, \"str\"]");

            JsonFilter filter;
            filter[0] = true;
            filter[1]["foo"] = true;
            filter[2] = true;

            assertThrow<std::runtime_error>([&]()
                                            { filter["random_key"] = true; });

            auto data = parser.parse(&filter)->json();
            assertType(*data, Type::list);
            assertEqual(data->getIndex(0)->returnNumber(), float(1));
            assertType(*data->getIndex(1)->getKey("foo"), Type::number);
            assertEqual(*data->getIndex(2)->returnString(), std::string("str"));

            setMemoryWatchpoint();
        }
    };

    class EdgeAssignIndexToObjectInFilter : public JsonTestCase
    {
    public:
        EdgeAssignIndexToObjectInFilter() : JsonTestCase("EdgeAssignIndexToObjectInFilter") {}

        void test()
        {
            setMemoryWatchpoint();
            JsonFilter filter;
            filter["key1"] = true;
            filter["key2"]["foo"] = true;
            filter["key3"] = true;
            assertThrow<std::runtime_error>([&]()
                                            { filter[0] = true; });
            setMemoryWatchpoint();
        }
    };

    class EdgeFilterIsNotContinous : public JsonTestCase
    {
    public:
        EdgeFilterIsNotContinous() : JsonTestCase("EdgeFilterIsNotContinous") {}

        void test()
        {
            setMemoryWatchpoint();
            JsonParser parser("[1, {\"foo\": 123.5}, \"str\"]");

            JsonFilter filter;
            filter[0] = true;
            filter[2] = true;
            auto data = parser.parse(&filter)->json();
            assertListLenght(*data, 1);

            setMemoryWatchpoint();
        }
    };

    class EdgeEmptyData : public JsonTestCase
    {
    public:
        EdgeEmptyData() : JsonTestCase("EdgeEmptyData") {}

        void test()
        {
            setMemoryWatchpoint();
            JsonParser parser("");

            JsonFilter filter;
            filter[0] = true;
            filter[2] = true;

#if THROW_ON_JSON_NULLPTR
            assertThrow<std::runtime_error>([&]()
                                            { parser.parse(&filter)->json(); });
#endif

            setMemoryWatchpoint();
        }
    };

    // Hard tests
    class HardApiDataTest : public JsonTestCase
    {
    public:
        HardApiDataTest() : JsonTestCase("HardApiDataTest") {}

        void test()
        {
            setMemoryWatchpoint();
            String s(
                "{\"cod\":\"200\",\"message\":0,\"cnt\":40,\"list\":[{\"dt\":1704650400,\"main\":{\"temp\":-6.7,\"feels_like\":-13.23,\"temp_min\":-6.7,\"temp_max\":-5.91,\"pressure\":1021,\"sea_level\":1021,\"grnd_level\":1005,\"humidity\":78,\"temp_kf\":-0.79},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":4.91,\"deg\":16,\"gust\":7.55},\"visibility\":10000,\"pop\":0.16,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-07 18:00:00\"},{\"dt\":1704661200,\"main\":{\"temp\":-6.71,\"feels_like\":-12.99,\"temp_min\":-6.72,\"temp_max\":-6.71,\"pressure\":1022,\"sea_level\":1022,\"grnd_level\":1007,\"humidity\":79,\"temp_kf\":0.01},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":4.57,\"deg\":12,\"gust\":6.99},\"visibility\":10000,\"pop\":0.12,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-07 21:00:00\"},{\"dt\":1704672000,\"main\":{\"temp\":-7.21,\"feels_like\":-13.51,\"temp_min\":-7.46,\"temp_max\":-7.21,\"pressure\":1023,\"sea_level\":1023,\"grnd_level\":1008,\"humidity\":79,\"temp_kf\":0.25},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":98},\"wind\":{\"speed\":4.44,\"deg\":14,\"gust\":6.86},\"visibility\":10000,\"pop\":0.12,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 00:00:00\"},{\"dt\":1704682800,\"main\":{\"temp\":-7.55,\"feels_like\":-13.91,\"temp_min\":-7.55,\"temp_max\":-7.55,\"pressure\":1025,\"sea_level\":1025,\"grnd_level\":1009,\"humidity\":79,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":4.42,\"deg\":17,\"gust\":6.91},\"visibility\":10000,\"pop\":0.03,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 03:00:00\"},{\"dt\":1704693600,\"main\":{\"temp\":-7.86,\"feels_like\":-14.1,\"temp_min\":-7.86,\"temp_max\":-7.86,\"pressure\":1027,\"sea_level\":1027,\"grnd_level\":1011,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":4.18,\"deg\":20,\"gust\":7.12},\"visibility\":10000,\"pop\":0.03,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 06:00:00\"},{\"dt\":1704704400,\"main\":{\"temp\":-7.25,\"feels_like\":-13.87,\"temp_min\":-7.25,\"temp_max\":-7.25,\"pressure\":1029,\"sea_level\":1029,\"grnd_level\":1012,\"humidity\":74,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04d\"}],\"clouds\":{\"all\":86},\"wind\":{\"speed\":4.84,\"deg\":34,\"gust\":7.22},\"visibility\":10000,\"pop\":0.02,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-08 09:00:00\"},{\"dt\":1704715200,\"main\":{\"temp\":-5.88,\"feels_like\":-12.21,\"temp_min\":-5.88,\"temp_max\":-5.88,\"pressure\":1029,\"sea_level\":1029,\"grnd_level\":1013,\"humidity\":64,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"zachmurzenie umiarkowane\",\"icon\":\"04d\"}],\"clouds\":{\"all\":75},\"wind\":{\"speed\":4.92,\"deg\":39,\"gust\":6.81},\"visibility\":10000,\"pop\":0.01,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-08 12:00:00\"},{\"dt\":1704726000,\"main\":{\"temp\":-6.97,\"feels_like\":-12.87,\"temp_min\":-6.97,\"temp_max\":-6.97,\"pressure\":1031,\"sea_level\":1031,\"grnd_level\":1015,\"humidity\":74,\"temp_kf\":0},\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"zachmurzenie małe\",\"icon\":\"03d\"}],\"clouds\":{\"all\":28},\"wind\":{\"speed\":4.03,\"deg\":27,\"gust\":7.24},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-08 15:00:00\"},{\"dt\":1704736800,\"main\":{\"temp\":-7.82,\"feels_like\":-13.27,\"temp_min\":-7.82,\"temp_max\":-7.82,\"pressure\":1033,\"sea_level\":1033,\"grnd_level\":1016,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":801,\"main\":\"Clouds\",\"description\":\"pochmurnie\",\"icon\":\"02n\"}],\"clouds\":{\"all\":21},\"wind\":{\"speed\":3.35,\"deg\":34,\"gust\":6.81},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 18:00:00\"},{\"dt\":1704747600,\"main\":{\"temp\":-8.45,\"feels_like\":-13.06,\"temp_min\":-8.45,\"temp_max\":-8.45,\"pressure\":1034,\"sea_level\":1034,\"grnd_level\":1018,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":2.52,\"deg\":35,\"gust\":5.14},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-08 21:00:00\"},{\"dt\":1704758400,\"main\":{\"temp\":-9.22,\"feels_like\":-13.7,\"temp_min\":-9.22,\"temp_max\":-9.22,\"pressure\":1035,\"sea_level\":1035,\"grnd_level\":1019,\"humidity\":83,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":2.33,\"deg\":30,\"gust\":4.98},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 00:00:00\"},{\"dt\":1704769200,\"main\":{\"temp\":-9.76,\"feels_like\":-13.35,\"temp_min\":-9.76,\"temp_max\":-9.76,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":85,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":4},\"wind\":{\"speed\":1.73,\"deg\":37,\"gust\":3.05},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 03:00:00\"},{\"dt\":1704780000,\"main\":{\"temp\":-10.14,\"feels_like\":-10.14,\"temp_min\":-10.14,\"temp_max\":-10.14,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":4},\"wind\":{\"speed\":1.07,\"deg\":78,\"gust\":1.34},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 06:00:00\"},{\"dt\":1704790800,\"main\":{\"temp\":-7.59,\"feels_like\":-10.53,\"temp_min\":-7.59,\"temp_max\":-7.59,\"pressure\":1038,\"sea_level\":1038,\"grnd_level\":1021,\"humidity\":75,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":4},\"wind\":{\"speed\":1.56,\"deg\":125,\"gust\":2.78},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-09 09:00:00\"},{\"dt\":1704801600,\"main\":{\"temp\":-5.29,\"feels_like\":-7.53,\"temp_min\":-5.29,\"temp_max\":-5.29,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":65,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":3},\"wind\":{\"speed\":1.37,\"deg\":109,\"gust\":2.58},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-09 12:00:00\"},{\"dt\":1704812400,\"main\":{\"temp\":-7.38,\"feels_like\":-7.38,\"temp_min\":-7.38,\"temp_max\":-7.38,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":82,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":2},\"wind\":{\"speed\":0.9,\"deg\":250,\"gust\":1.02},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-09 15:00:00\"},{\"dt\":1704823200,\"main\":{\"temp\":-8.82,\"feels_like\":-8.82,\"temp_min\":-8.82,\"temp_max\":-8.82,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":89,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":3},\"wind\":{\"speed\":1.28,\"deg\":241,\"gust\":1.24},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 18:00:00\"},{\"dt\":1704834000,\"main\":{\"temp\":-9.37,\"feels_like\":-9.37,\"temp_min\":-9.37,\"temp_max\":-9.37,\"pressure\":1037,\"sea_level\":1037,\"grnd_level\":1020,\"humidity\":90,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":1.07,\"deg\":202,\"gust\":1.02},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-09 21:00:00\"},{\"dt\":1704844800,\"main\":{\"temp\":-9.53,\"feels_like\":-12.45,\"temp_min\":-9.53,\"temp_max\":-9.53,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":88,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":1.42,\"deg\":188,\"gust\":1.36},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 00:00:00\"},{\"dt\":1704855600,\"main\":{\"temp\":-9.8,\"feels_like\":-13.52,\"temp_min\":-9.8,\"temp_max\":-9.8,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":86,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":1.8,\"deg\":173,\"gust\":1.78},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 03:00:00\"},{\"dt\":1704866400,\"main\":{\"temp\":-9.88,\"feels_like\":-13.7,\"temp_min\":-9.88,\"temp_max\":-9.88,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":83,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":1.85,\"deg\":167,\"gust\":1.81},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 06:00:00\"},{\"dt\":1704877200,\"main\":{\"temp\":-7.14,\"feels_like\":-11.04,\"temp_min\":-7.14,\"temp_max\":-7.14,\"pressure\":1036,\"sea_level\":1036,\"grnd_level\":1019,\"humidity\":69,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":2},\"wind\":{\"speed\":2.18,\"deg\":176,\"gust\":3.03},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-10 09:00:00\"},{\"dt\":1704888000,\"main\":{\"temp\":-3.91,\"feels_like\":-6.07,\"temp_min\":-3.91,\"temp_max\":-3.91,\"pressure\":1035,\"sea_level\":1035,\"grnd_level\":1019,\"humidity\":60,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":1},\"wind\":{\"speed\":1.42,\"deg\":151,\"gust\":1.62},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-10 12:00:00\"},{\"dt\":1704898800,\"main\":{\"temp\":-6.31,\"feels_like\":-9.39,\"temp_min\":-6.31,\"temp_max\":-6.31,\"pressure\":1034,\"sea_level\":1034,\"grnd_level\":1017,\"humidity\":80,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":0},\"wind\":{\"speed\":1.74,\"deg\":182,\"gust\":1.69},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-10 15:00:00\"},{\"dt\":1704909600,\"main\":{\"temp\":-7.5,\"feels_like\":-9.97,\"temp_min\":-7.5,\"temp_max\":-7.5,\"pressure\":1033,\"sea_level\":1033,\"grnd_level\":1017,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":2},\"wind\":{\"speed\":1.34,\"deg\":185,\"gust\":1.32},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 18:00:00\"},{\"dt\":1704920400,\"main\":{\"temp\":-7.79,\"feels_like\":-7.79,\"temp_min\":-7.79,\"temp_max\":-7.79,\"pressure\":1032,\"sea_level\":1032,\"grnd_level\":1016,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":7},\"wind\":{\"speed\":0.85,\"deg\":197,\"gust\":0.82},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-10 21:00:00\"},{\"dt\":1704931200,\"main\":{\"temp\":-7.81,\"feels_like\":-10.66,\"temp_min\":-7.81,\"temp_max\":-7.81,\"pressure\":1031,\"sea_level\":1031,\"grnd_level\":1015,\"humidity\":81,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":10},\"wind\":{\"speed\":1.5,\"deg\":244,\"gust\":1.44},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 00:00:00\"},{\"dt\":1704942000,\"main\":{\"temp\":-6.82,\"feels_like\":-10.65,\"temp_min\":-6.82,\"temp_max\":-6.82,\"pressure\":1029,\"sea_level\":1029,\"grnd_level\":1013,\"humidity\":79,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":5},\"wind\":{\"speed\":2.17,\"deg\":251,\"gust\":3.15},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 03:00:00\"},{\"dt\":1704952800,\"main\":{\"temp\":-5.95,\"feels_like\":-10.63,\"temp_min\":-5.95,\"temp_max\":-5.95,\"pressure\":1028,\"sea_level\":1028,\"grnd_level\":1012,\"humidity\":74,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01n\"}],\"clouds\":{\"all\":7},\"wind\":{\"speed\":2.98,\"deg\":270,\"gust\":7.31},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 06:00:00\"},{\"dt\":1704963600,\"main\":{\"temp\":-2.82,\"feels_like\":-8.21,\"temp_min\":-2.82,\"temp_max\":-2.82,\"pressure\":1026,\"sea_level\":1026,\"grnd_level\":1010,\"humidity\":73,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"zachmurzenie umiarkowane\",\"icon\":\"04d\"}],\"clouds\":{\"all\":71},\"wind\":{\"speed\":4.65,\"deg\":278,\"gust\":10.91},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-11 09:00:00\"},{\"dt\":1704974400,\"main\":{\"temp\":-0.01,\"feels_like\":-5.21,\"temp_min\":-0.01,\"temp_max\":-0.01,\"pressure\":1025,\"sea_level\":1025,\"grnd_level\":1009,\"humidity\":78,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04d\"}],\"clouds\":{\"all\":85},\"wind\":{\"speed\":5.51,\"deg\":290,\"gust\":11.31},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-11 12:00:00\"},{\"dt\":1704985200,\"main\":{\"temp\":-1.03,\"feels_like\":-6.41,\"temp_min\":-1.03,\"temp_max\":-1.03,\"pressure\":1024,\"sea_level\":1024,\"grnd_level\":1009,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"zachmurzenie umiarkowane\",\"icon\":\"04d\"}],\"clouds\":{\"all\":76},\"wind\":{\"speed\":5.36,\"deg\":306,\"gust\":10.72},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-11 15:00:00\"},{\"dt\":1704996000,\"main\":{\"temp\":-0.82,\"feels_like\":-6.1,\"temp_min\":-0.82,\"temp_max\":-0.82,\"pressure\":1025,\"sea_level\":1025,\"grnd_level\":1009,\"humidity\":96,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":85},\"wind\":{\"speed\":5.27,\"deg\":309,\"gust\":10.83},\"visibility\":461,\"pop\":0,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 18:00:00\"},{\"dt\":1705006800,\"main\":{\"temp\":-0.4,\"feels_like\":-5.16,\"temp_min\":-0.4,\"temp_max\":-0.4,\"pressure\":1026,\"sea_level\":1026,\"grnd_level\":1011,\"humidity\":96,\"temp_kf\":0},\"weather\":[{\"id\":600,\"main\":\"Snow\",\"description\":\"słabe opady śniegu\",\"icon\":\"13n\"}],\"clouds\":{\"all\":99},\"wind\":{\"speed\":4.58,\"deg\":331,\"gust\":9.62},\"visibility\":1894,\"pop\":0.59,\"snow\":{\"3h\":0.34},\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-11 21:00:00\"},{\"dt\":1705017600,\"main\":{\"temp\":-0.58,\"feels_like\":-5.21,\"temp_min\":-0.58,\"temp_max\":-0.58,\"pressure\":1027,\"sea_level\":1027,\"grnd_level\":1012,\"humidity\":88,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":4.31,\"deg\":340,\"gust\":9.01},\"visibility\":10000,\"pop\":0.43,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-12 00:00:00\"},{\"dt\":1705028400,\"main\":{\"temp\":-1.32,\"feels_like\":-6.01,\"temp_min\":-1.32,\"temp_max\":-1.32,\"pressure\":1029,\"sea_level\":1029,\"grnd_level\":1013,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"zachmurzenie duże\",\"icon\":\"04n\"}],\"clouds\":{\"all\":100},\"wind\":{\"speed\":4.14,\"deg\":339,\"gust\":8.63},\"visibility\":10000,\"pop\":0.1,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-12 03:00:00\"},{\"dt\":1705039200,\"main\":{\"temp\":-3.83,\"feels_like\":-7.52,\"temp_min\":-3.83,\"temp_max\":-3.83,\"pressure\":1030,\"sea_level\":1030,\"grnd_level\":1014,\"humidity\":95,\"temp_kf\":0},\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"zachmurzenie umiarkowane\",\"icon\":\"04n\"}],\"clouds\":{\"all\":76},\"wind\":{\"speed\":2.47,\"deg\":347,\"gust\":5.07},\"visibility\":10000,\"pop\":0.1,\"sys\":{\"pod\":\"n\"},\"dt_txt\":\"2024-01-12 06:00:00\"},{\"dt\":1705050000,\"main\":{\"temp\":-2.26,\"feels_like\":-5.62,\"temp_min\":-2.26,\"temp_max\":-2.26,\"pressure\":1032,\"sea_level\":1032,\"grnd_level\":1016,\"humidity\":87,\"temp_kf\":0},\"weather\":[{\"id\":801,\"main\":\"Clouds\",\"description\":\"pochmurnie\",\"icon\":\"02d\"}],\"clouds\":{\"all\":11},\"wind\":{\"speed\":2.43,\"deg\":352,\"gust\":4.21},\"visibility\":10000,\"pop\":0.02,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-12 09:00:00\"},{\"dt\":1705060800,\"main\":{\"temp\":-0.94,\"feels_like\":-3.96,\"temp_min\":-0.94,\"temp_max\":-0.94,\"pressure\":1032,\"sea_level\":1032,\"grnd_level\":1016,\"humidity\":71,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":9},\"wind\":{\"speed\":2.34,\"deg\":325,\"gust\":3.48},\"visibility\":10000,\"pop\":0.01,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-12 12:00:00\"},{\"dt\":1705071600,\"main\":{\"temp\":-3.62,\"feels_like\":-5.68,\"temp_min\":-3.62,\"temp_max\":-3.62,\"pressure\":1032,\"sea_level\":1032,\"grnd_level\":1016,\"humidity\":86,\"temp_kf\":0},\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"bezchmurnie\",\"icon\":\"01d\"}],\"clouds\":{\"all\":7},\"wind\":{\"speed\":1.39,\"deg\":288,\"gust\":1.52},\"visibility\":10000,\"pop\":0,\"sys\":{\"pod\":\"d\"},\"dt_txt\":\"2024-01-12 15:00:00\"}],\"city\":{\"id\":7532481,\"name\":\"Oława\",\"coord\":{\"lat\":50.9571,\"lon\":17.2903},\"country\":\"PL\",\"population\":0,\"timezone\":3600,\"sunrise\":1704610351,\"sunset\":1704639641}}");

            setMemoryWatchpoint("Init parser");
            JsonParser parser(s.c_str());
            setMemoryWatchpoint("Init parser");

            constexpr int LOOP = 40;
            JsonFilter filter;

            setMemoryWatchpoint("Filter");
            for (int i = 0; i < LOOP; i++)
            {
                filter["list"][i]["weather"][0]["main"] = true;
                filter["list"][i]["weather"][0]["icon"] = true;
                filter["list"][i]["main"]["temp"] = true;
                filter["list"][i]["main"]["feels_like"] = true;
                filter["list"][i]["main"]["pressure"] = true;
                filter["list"][i]["main"]["humidity"] = true;
                filter["list"][i]["wind"]["speed"] = true;
                filter["list"][i]["dt"] = true;
                filter["list"][i]["pop"] = true;
            }
            setMemoryWatchpoint("Filter");

            setMemoryWatchpoint("***Parse");
            auto data = parser.parse(&filter)->json();
            setMemoryWatchpoint("***Parse");

            for (int i = 0; i < LOOP; i++)
            {
                auto watchpoint = std::string("Parsed data - ") + std::to_string(i);
                setMemoryWatchpoint(watchpoint.c_str());
                assertType((*data)["list"][i]["weather"][0]["icon"], Type::string);
                assertType((*data)["list"][i]["weather"][0]["main"], Type::string);

                assertType(*data->getKey("list")->getIndex(i)->getKey("main")->getKey("temp"), Type::number);
                assertType(*data->getKey("list")->getIndex(i)->getKey("main")->getKey("feels_like"), Type::number);
                assertType(*data->getKey("list")->getIndex(i)->getKey("main")->getKey("pressure"), Type::number);
                assertType(*data->getKey("list")->getIndex(i)->getKey("main")->getKey("humidity"), Type::number);

                assertType(*data->getKey("list")->getIndex(i)->getKey("wind")->getKey("speed"), Type::number);

                assertType((*data)["list"][0]["dt"], Type::number);
                assertType((*data)["list"][0]["pop"], Type::number);
                setMemoryWatchpoint(watchpoint.c_str());
            }

            assertListLenght((*data)["list"], LOOP);

            setMemoryWatchpoint();
        }
    };

    // memory tests
    class MemoryJsonNodeTest : public JsonTestCase
    {
    public:
        MemoryJsonNodeTest() : JsonTestCase("MemoryJsonNodeTest") {}

        void test()
        {
            setMemoryWatchpoint();
            JsonParser parser("[1, {\"foo\": 123.5}, \"str\"]");

            {
                setMemoryWatchpoint("JsonFilter");
                JsonFilter filter;
                filter[0] = true;
                filter[1] = true;
                filter[2] = true;
                setMemoryWatchpoint("JsonFilter");
            }

            {
                setMemoryWatchpoint("JsonParser");
                size_t mem = ESP.getFreeHeap();
                auto data = parser.parse()->json();
                setMemoryWatchpoint("JsonParser");
                assertListLenght(*data, 3);
            }

            setMemoryWatchpoint();
        }
    };

    // custom stream tests
    class StreamBasicTest : public JsonTestCase
    {
    public:
        StreamBasicTest() : JsonTestCase("StreamBasicTest") {}

        void test()
        {
            const char *STR = "data";
            stream ss((char *)STR);

            int size = strlen(STR);
            int i = 0;
            while (!ss.eof())
            {
                if (i != size)
                {
                    auto peek = ss.peek();
                    Serial.printf("\n ->%c %c<- (%i) \n", STR[i], peek, int(peek));
                    assertEqual(STR[i], peek, " (char) %c != %c");
                }
                else
                {
                    assertEqual(BAD_BIT, (int)ss.peek(), " %i != %i");
                }

                char c;
                ss.get(c);
                assertEqual(c, STR[i]);

                i++;
            }
            assertTrue(ss.eof());
            assertFalse(ss.good());

            ss.set("falseF");
            char c;
            ss.get(c);
            assertTrue(c == 'f');
            ss.seekg(4, stream_pos::cur);
            ss.get(c);
            assertTrue(c == 'F');
        }
    };

    // ptr_vector
    class PtrVectorTest : public JsonTestCase
    {
    public:
        PtrVectorTest() : JsonTestCase("PtrVectorTest") {}

        void test()
        {
            _memoryWatchpoints.reserve(4);
            setMemoryWatchpoint();
            setMemoryWatchpoint("empty dealloc");
            {
                ptr_vector<JsonNode> vec;
            }
            setMemoryWatchpoint("empty dealloc");

            ptr_vector<JsonNode> vec;
            assertThrow<std::out_of_range>([&]()
                                           { vec.at(0); });
            assertEqual(vec.size(), 0);
            vec.push_back();
            assertEqual(vec.size(), 1);
            vec[0]->setNumber(0.5f);

            setMemoryWatchpoint();
        }
    };

    class PtrVectorTestHard : public JsonTestCase
    {
    public:
        PtrVectorTestHard() : JsonTestCase("PtrVectorTestHard") {}

        void test()
        {
            setMemoryWatchpoint();

            ptr_vector<JsonNode> vec;

            constexpr auto LOOP = 100;
            for (int i = 0; i < LOOP; i++){
                vec.push_back();
            }
            
            assertEqual(vec.size(), LOOP);
            setMemoryWatchpoint();
        }
    };

*/
// ---------------------------------------------------------------

}