#include "TestCase.h"

namespace tests
{
    TestCase::TestCase(std::string name):
     name(name), _failed(false) {}

    void TestCase::run(){
        _testWrapper();
    }

    bool TestCase::failed(){
        return _failed;
    }

    // private / protected methods

    void TestCase::_successfulPass(){
        Serial.printf("^^^^^^^^^^^^Successfuly passed %s ^^^^^^^^^^^^\n", name.c_str());
    }

    void TestCase::_onFail(const std::runtime_error& err){
        Serial.printf("XXXXXX Test failed: %s XXXXXX\n\t Reason: %s\n", name.c_str(), err.what());
    }

    void TestCase::_startTest(){
        Serial.printf("\n--------------Testing: %s --------------\n", name.c_str());
    }

    void TestCase::_report(long long timeNs, long long timeMs, size_t memoryDiff){
        Serial.printf("\n***Report***\n\tFinished in: %i ns ( %i ms)\n\tMemory difference: %i B\n",
                timeNs, timeMs, memoryDiff);
    }

    void TestCase::_testWrapper(){
        _startTest();

        try{
            auto memoryStart = ESP.getFreeHeap();
            auto startNs = micros();
            test();
            auto timeNs = micros() - startNs;
            _report(timeNs, long(millis() - (timeNs / 1000)), memoryStart - ESP.getFreeHeap());
            _successfulPass();
        } catch(const std::runtime_error& e){
            _onFail(e);
            _failed = true;
        }
    }

    void TestCase::_assert(bool c, std::string assertName){
        if (!c){
            throw std::runtime_error("Assert failed: " + assertName);
        }
    }

    void TestCase::assertTrue(bool c){
        _assert(c, "assertTrue");
    }

    void TestCase::assertFalse(bool c){
        _assert(!c, "assertFalse");
    }

    void TestCase::assertThrow(std::function<void(void)> throableFunction){
        bool threw = false;
        __try {
            throableFunction();
        } __catch(...) {
            threw = true;
        }
        _assert(threw, "assertThrow");
    }

}