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
}

ulong tick_num = 0;

void loop()
{
    // This example shows how to measure time between ticks in microseconds (also available in milliseconds)
    // It delays for a time from 0 to 4 ms
    // Every 1000 ticks it prints the statistics of the time between ticks (appox. 2 seconds)

    MEASURE_INTERVAL_US("Time between ticks");
    delay(tick_num % 5);

    tick_num++;
    if (tick_num % 1000 == 0)
    {
        auto value = GET_INTERVAL_VALUE("Time between ticks");
        Log::Error("Time between ticks: Samples:[ Num %d ], Time: [ Total: %d us, Min: %d us, Max: %d us, Avg: %d us. ]",
            value.Count(), value.Total(), value.Min(), value.Max(), value.Avg());

        RESET_INTERVAL("Time between ticks");
    }

 /**
  * Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002719 us, Min: 4 us, Max: 4037 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002085 us, Min: 4 us, Max: 4007 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002553 us, Min: 5 us, Max: 4032 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002086 us, Min: 5 us, Max: 4007 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002095 us, Min: 5 us, Max: 4007 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002220 us, Min: 5 us, Max: 4021 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002075 us, Min: 5 us, Max: 4007 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002053 us, Min: 4 us, Max: 4007 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002182 us, Min: 5 us, Max: 4017 us, Avg: 2004 us. ]
    Time between ticks: Samples:[ Num 999 ], Time: [ Total: 2002089 us, Min: 5 us, Max: 4009 us, Avg: 2004 us. ]
  */
}