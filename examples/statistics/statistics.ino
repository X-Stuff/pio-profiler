#include <Arduino.h>

// Put it into platformio.ini as a build flag:
// build_flags = -DENABLE_PROFILE=1
#define ENABLE_PROFILE 1


// Put it into platformio.ini as a build flag:
// build_flags = '-DPROFILER_LOG_FUNCTION=printf'
#ifndef PROFILER_LOG_FUNCTION
#define PROFILER_LOG_FUNCTION printf
#endif

#include "profiler.h"

void setup()
{
    // This shows how to measure statistics of time for events that may consits from multiple tasks.
    // e.g.: you want to measure transfer time of data from one device to another
    // first delay imitates data receiving, second delay imitates data sending

    StatValue<ulong> MeasureMS;
    {
        MEASURE_TIME_MS(MeasureMS);
        delay(10);
    }
    {
        MEASURE_TIME_MS(MeasureMS);
        delay(33);
    }

    // This will print:
    //     [Statistics: Samples:[ Num 2 ], Time: [ Total: 43 ms, Min: 10 ms, Max: 33 ms, Avg: 21 ms. ]
    printf("Statistics: Samples:[ Num %d ], Time: [ Total: %d ms, Min: %d ms, Max: %d ms, Avg: %d ms. ]",
        MeasureMS.Count(), MeasureMS.Total(), MeasureMS.Min(), MeasureMS.Max(), MeasureMS.Avg());

    // This will reset value to 0 and all statistics
    MeasureMS = 0;
}

void loop()
{

}