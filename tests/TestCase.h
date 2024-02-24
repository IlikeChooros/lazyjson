#pragma once

#include <Arduino.h>

namespace tests
{
    class TestCase
    {

    public:
        TestCase(std::string name = "TestCase");
        void run();
        bool failed();
        virtual void test() {}

    protected:
        std::string name;
        bool _failed;

        void _onFail(const std::runtime_error &);
        void _successfulPass();
        void _startTest();
        void _testWrapper();
        virtual void _report(long long timeNs, long long timeMs, size_t memoryDiff);

        void _assert(bool condition, std::string assertName);

        void assertTrue(bool condition);
        void assertFalse(bool condition);
        void assertThrow(std::function<void(void)> throableFunction);

        template <class T>
        void assertEqual(const T &o1, const T &o2)
        {
            _assert(o1 == o2, "assertEqual");
        }

        template <class ExcepctionType>
        void assertThrow(std::function<void(void)> throableFunction)
        {
            bool threw = false;
            try
            {
                throableFunction();
            }
            catch (const ExcepctionType &err)
            {
                threw = true;
                Serial.printf("\nThrew: %s\n", err.what());
            }
            _assert(threw, "assertThrow");
        }
    };

}