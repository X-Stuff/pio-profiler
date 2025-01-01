# Simple Profiling Library

Allows quickly put macros in critical places in code to get log prints if execution time is over the theshold.
Allows collect statistics of time measurements.

# Usage

**platformio.ini**
```
lib_deps =
    x-stuff/SimpleProfiler@1.0.1

build_flags =
    -DENABLE_PROFILING=1
    '-DPROFILER_LOG_FUNCTION=my_log'
```

**main.cpp**
```
#include <profiler.h>

void loop()
{
    {
        // Print output if MyAction() took more than 10ms
        NAME_SCOPED_COUNTER("my_action", 10);

        MyAction();
    }

    {
        // Collect time statistics for Process() function
        StatValue<ulong> Stats;

        for (auto& item : collection)
        {
            MEASURE_TIME_MS(Stats);
            Process(Item);
        }

        printf("Process() Statistics: Samples:[ Num %d ], Time: [ Total: %d ms, Min: %d ms, Max: %d ms, Avg: %d ms. ]",
            Stats.Count(), Stats.Total(), Stats.Min(), Stats.Max(), Stats.Avg());
    }
}
```