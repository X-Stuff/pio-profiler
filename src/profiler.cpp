// Special hack to be able to compile this file
// This file is only exists because of static maps for interval measurements
#ifdef PROFILER_LOG_FUNCTION
    #undef PROFILER_LOG_FUNCTION
    #define PROFILER_LOG_FUNCTION(...)
#endif

#include "profiler.h"

/** Definition of values */
std::map<const char*, StatValue<unsigned long>> IntervalMeasureTime::Values;

/** Definition of timestamp */
std::map<const char*, unsigned long> IntervalMeasureTime::Timestamps;
