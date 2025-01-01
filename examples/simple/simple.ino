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
    // This example shows how put a very fast canary code, that will print logs:
    //  - if some block code is too slow - `NAME_SCOPED_COUNTER`
    //  - to show how long it took to execute the block of code - `AUTO_SCOPED_COUNTER` (if executtion time is greater than 1 ms - minimal detection threshold)

    // This will print:
    //  [setup] Elapsed time: 21 ms
    //  [simple\simple.ino: line: 21]
    AUTO_SCOPED_COUNTER;

    {
        // This will not print anything since silent threshold is greater than delay (10 ms > 1 ms)
        NAME_SCOPED_COUNTER("Test-1", 10);
        delay(1);
    }

    {
        // This will print:
        //  [Test-2] Elapsed time: 20 ms
        //  [simple\simple.ino: line: 33]
        NAME_SCOPED_COUNTER("Test-2", 10);
        delay(20);
    }
}

void loop()
{

}