#include "testCases.h"

namespace tests
{
    using namespace lazyjson;

    void JsonTestCase::assertLazyType(const LazyTypedValues &node, const LazyType &type)
    {
        if (node.type != type)
        {
            throw std::runtime_error("assertType: " + lazyTypeError(node, type));
        }
    }

    void JsonTestCase::_report(long long timeNs, long long timeMs, size_t memoryDiff)
    {
        size_t free = ESP.getFreeHeap();
        int percentage = (float(free) / float(ESP.getHeapSize())) * 100;
        TestCase::_report(timeNs, timeMs, memoryDiff);
        Serial.printf("### MEMORY ###\n");
        for (int i = 0; i < _memoryWatchpoints.size(); i++)
        {
            Serial.printf("\t%s:\n\t\tUsed memory %i B\n\t\tMemory left %i B (%i %%)\n\t\tStart memory: %i B\n\t\tEnd memory %i B\n",
                          _memoryWatchpoints[i].name.c_str(),
                          size_t(_memoryWatchpoints[i].startMem - _memoryWatchpoints[i].endMem),
                          free, percentage,
                          _memoryWatchpoints[i].startMem,
                          _memoryWatchpoints[i].endMem);
        }
    }

    void JsonTestCase::setMemoryWatchpoint(const char *watchpoint)
    {
        if (watchpoint == "%testname")
        {
            watchpoint = name.c_str();
        }
        int index = -1;
        for (int i = 0; i < _memoryWatchpoints.size(); i++)
        {
            if (_memoryWatchpoints[i].name == watchpoint)
            {
                index = i;
                break;
            }
        }
        if (index < 0)
        {
            struct MemoryWatchpoint w;
            w.startMem = ESP.getFreeHeap();
            w.name = watchpoint;
            _memoryWatchpoints.push_back(w);
        }
        else
        {
            _memoryWatchpoints[index].endMem = ESP.getFreeHeap();
        }
    }
}