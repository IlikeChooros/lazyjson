#include "tests.h"

namespace tests
{
    using namespace lazyjson;

    void root()
    {

        auto rootMemory = ESP.getFreeHeap();

        {
            using testBase = std::shared_ptr<TestCase>;
            std::vector<testBase> tests = {
                // simple
                // testBase(new SimpleNumberParse()),
                // testBase(new SimpleBooleanParse()),
                // testBase(new SimpleStringParse()),

                // // normal
                // testBase(new NumberParse()),

                // testBase(new ObjectParse()),
                // testBase(new ObjectWithNumbers()),

                // // testBase(new ListParse()),
                // testBase(new ListWithNumbers()),

                // empty
                // testBase(new EmptyList()),
                // testBase(new EmptyObject()),

                // // complex
                // testBase(new ComplexObjectParse()),
                // testBase(new ComplexRealDataParse()),
                // testBase(new ComplexApiWeatherData()),
                // testBase(new ComplexApiForecastData()),

                // // edge
                // testBase(new EdgeAssignKeyToListInFilter()),
                // testBase(new EdgeAssignIndexToObjectInFilter()),
                // testBase(new EdgeFilterIsNotContinous()),
                // testBase(new EdgeEmptyData()),

                // // hard tests
                // testBase(new HardApiDataTest()),

                // // memory
                // testBase(new MemoryJsonNodeTest()),

                // // stream
                // testBase(new StreamBasicTest()),

                // // ptr_vector
                // testBase(new PtrVectorTest()),
                // testBase(new PtrVectorTestHard()),
                // testBase(new LazyParserNumberTest()),
                // testBase(new LazyParserBoolTest()),
                // testBase(new LazyParserStringTest()),
                // testBase(new LazyParserObjectTest()),
                // testBase(new LazyParserObjectWithNumbersTest()),
                // testBase(new LazyParserListWithNumbersTest()),
                // testBase(new LazyParserComplexApiWeatherData()),

                // extractor
                // testBase(new LazyExtractorNumberTest()),
                // testBase(new LazyExtractorBoolTest()),
                // testBase(new LazyExtractorStringTest()),
                // testBase(new LazyExtractorObjectTest()),
                testBase(new LazyExtractorObjectWithNumbersTest()),
                testBase(new LazyExtractorListWithNumbersTest()),
                testBase(new LazyExtractorExampleTest()),
                testBase(new LazyExtractorForecastApiData()),
                testBase(new LazyExtractorComplexApiWeatherData()),
            };

            char* buff;
            const char* s = buff;

            

            int failed = 0;
            auto beforeMemory = ESP.getFreeHeap();

            for (auto test : tests)
            {
                test->run();
                if (test->failed())
                {
                    failed++;
                }
            }
            Serial.printf(
                "\n********** SUMMARY **********\n\t"
                "Total of tests: %i\n\t"
                "Failed: %i\n\t"
                "Memory Diff: %i B\n\t"
                "Start memory: %i B\n\t"
                "End memory: %i B\n\t",
                tests.size(),
                failed,
                size_t(beforeMemory - ESP.getFreeHeap()),
                beforeMemory,
                size_t(ESP.getFreeHeap()));
        }

        auto afterMemory = ESP.getFreeHeap();
        Serial.printf("\n\nRoot memory diff: %u B\n", size_t(rootMemory - afterMemory));
    }
        
}