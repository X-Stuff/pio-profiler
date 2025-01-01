#pragma once

#include <limits>
#include <map>
#include <type_traits>

extern "C" unsigned long micros();
extern "C" unsigned long millis();

#ifndef NEW_LINE
#define NEW_LINE "\r\n"
#endif

/**
 *   Wrapper for statistical measurement.
 * Can be used for any type of value, e.g. time, memory, etc.
 *
 * Contains Min, Max, Avg, and Total values
 */
template<typename TValue> struct StatValue
{
public:

    /** Total accumulated value */
    inline TValue Total() const { return Count() > 0 ? Value : 0; }

    /** Min value value */
    inline TValue Min() const { return Count() > 0 ? MinDelta : 0; }

    /** Max value */
    inline TValue Max() const { return Count() > 0 ? MaxDelta : 0; }

    /** Number of increments */
    inline TValue Count() const { return NumInc; }

    /** Average value */
    inline TValue Avg() const { return Count() > 0 ? Value / Count() : Value; }

    StatValue& operator=(TValue value)
    {
        Value = value;
        NumInc = 0;
        MinDelta = std::numeric_limits<TValue>::max();
        MaxDelta = std::numeric_limits<TValue>::min();

        return *this;
    }

    StatValue& operator+=(TValue delta)
    {
        if (delta != 0)
        {
            NumInc++;
            Value += delta;

            if (delta > MaxDelta)
            {
                MaxDelta = delta;
            }

            if (delta < MinDelta)
            {
                MinDelta = delta;
            }
        }

        return *this;
    }

private:
    TValue Value = 0;

    TValue MinDelta = std::numeric_limits<TValue>::max();

    TValue MaxDelta= std::numeric_limits<TValue>::min();

    unsigned int NumInc = 0;
};

/**
 *  Timer type fir Scoped measurement
 */
enum class Timer
{
    /** Milliseconds  */
    Millis = 0,

    /** Microseconds (precise) */
    Micros = 1,
};

/**
 * Scoped measure time helper, measures ms or us and increase target value by it
 */
template<Timer TTimer, typename TValue> struct ScopedMeasureTime
{
    ScopedMeasureTime() = delete;
    ScopedMeasureTime(const ScopedMeasureTime<TTimer, TValue>& other) = delete;

    ScopedMeasureTime(TValue& target) : Target(&target)
    {
        StartedAt = Now();
    }

    ~ScopedMeasureTime()
    {
        if (Target)
        {
            *Target += Now() - StartedAt;
        }
    }

private:

    inline unsigned long Now() const { return NowImpl(std::integral_constant<Timer, TTimer>{}); }

    /** */
    inline unsigned long NowImpl(std::integral_constant<Timer, Timer::Micros>) const { return micros(); }

    /** */
    inline unsigned long NowImpl(std::integral_constant<Timer, Timer::Millis>) const { return millis(); }

private:

    /** Where to add delta  */
    TValue* Target;

    /** Timestapm of the beginnig */
    unsigned long StartedAt;
};

/**
 *  Specialized class for measuring time in microseconds
 */
template<typename TValue> struct ScopedMeasureTimeMicros : public ScopedMeasureTime<Timer::Micros, TValue>
{
    ScopedMeasureTimeMicros(TValue& value) : ScopedMeasureTime<Timer::Micros, TValue>(value)
    {
    }
};

/**
 *  Specialized class for measuring time in milliseconds
 */
template<typename TValue> struct ScopedMeasureTimeMillis : public ScopedMeasureTime<Timer::Millis, TValue>
{
    ScopedMeasureTimeMillis(TValue& value) : ScopedMeasureTime<Timer::Millis, TValue>(value)
    {
    }
};


/**
 *  Measure time between calls.
 *
 * Put self into global static collections by unique name
 */
struct IntervalMeasureTime
{
    /** Measure time between ticks */
    template<bool Precise> static inline void Tick(const char* id)
    {
        auto now = Precise ? micros() : millis();
        if (Timestamps[id] > 0)
        {
            Values[id] += now - Timestamps[id];
        }
        Timestamps[id] = now;
    }

    /** Reset to 0 */
    inline static void Reset(const char* id)
    {
        Values[id] = 0;
        Timestamps[id] = 0;
    }

    /** Reset all intervals to 0 */
    inline static void Reset()
    {
        for (auto& [id, value] : Values)
        {
            value = 0;
            Timestamps[id] = 0;
        }
    }

    /** Return current value */
    inline static StatValue<unsigned long>& GetValue(const char* id) { return Values[id]; }

private:

    static std::map<const char*, StatValue<unsigned long>> Values;

    static std::map<const char*, unsigned long> Timestamps;
};

/**
 *  Measure and print elapsed lifetime of the object.
 * Used to print info if operation took too long, e.g:
 *
 *   {
 *      NAME_SCOPED_COUNTER("File Write", 10);
 *      file.write(data, size);
 *   }
 *
 * Will print "File Write: Elapsed time: XXX ms ..." only if it took more than 10 ms
 */
template<bool Precise> struct ScopedCounter
{
    ScopedCounter(const char* name) : ScopedCounter(name, nullptr, -1)
    {
    }

    ScopedCounter(const char* func = nullptr, const char* file = nullptr, int line = -1, unsigned long msSilent = 0) :
        Name(func), File(file), Line(line), SilentThrethold(msSilent)
    {
        StartTimestamp = Now();
    }

    ~ScopedCounter()
    {
        constexpr static auto Resolution = Precise ? "us" : "ms";

        auto delta = Now() - StartTimestamp;
        if (delta >= SilentThrethold)
        {
            if (Name != nullptr)
            {
                if (File != nullptr)
                {
                    PrintResults("[%s] Elapsed time: %d %s" NEW_LINE "[%s: line: %d]", Name, delta, Resolution, File, Line);
                }
                else
                {
                    PrintResults("[%s] Elapsed time: %d %s", Name, delta, Resolution);
                }
            }
            else
            {
                PrintResults("[UNKNOWN] Elapsed time: %d %s", delta, Resolution);
            }
        }
    }

private:
    inline unsigned long Now() const
    {
        if constexpr (Precise)
        {
            return micros();
        }
        else
        {
            return millis();
        }
    }

    template<typename... TArgs> inline void PrintResults(TArgs... args)
    {
#ifndef PROFILER_LOG_FUNCTION
    #if defined(ENABLE_PROFILE) || ENABLE_PROFILE
        #warning "PROFILER_LOG_FUNCTION is not defined, ScopedCounter will not print anything. Log function can be any printf-like function. e.g.: `my_print(const char*, ...)`. "
    #endif
#else
        PROFILER_LOG_FUNCTION(std::forward<TArgs>(args)...);
#endif
    }

private:
    /** Time in millis() or micros() when the object was contructed */
    unsigned long StartTimestamp;

    /** If elapsed time less that threshold - won't be print in destructor*/
    unsigned long SilentThrethold;

    /** Name of scope counter */
    const char* Name;

    /** File name to print */
    const char* File;

    /** Line of code to print */
    int Line;
};

#if defined(ENABLE_PROFILE) || ENABLE_PROFILE
    #define PROFILER_CONCAT(x, y) x ## y
    #define PROFILER_TOKENIZE(x, y) PROFILER_CONCAT(x, y)

    /**
     *  Measure time of scope block in milliseconds
     * @param x - StatValue<unsigned long> to accumulate time
     */
    #define MEASURE_TIME_MS(...) ScopedMeasureTimeMillis PROFILER_TOKENIZE(__counter_, __LINE__)(__VA_ARGS__)

    /**
     *  Measure time of scope block in microseconds
     * @param x - StatValue<unsigned long> to accumulate time
     */
    #define MEASURE_TIME_US(...) ScopedMeasureTimeMicros PROFILER_TOKENIZE(__counter_, __LINE__)(__VA_ARGS__)

    /**
     *  Measure time of scope block in milliseconds
     * @param x - StatValue<unsigned long> to accumulate time
     */
    #define MEASURE_TIME(x) MEASURE_TIME_MS(x)

    /**
     *  Measure time between calls in milliseconds
     * @param name - unique name of the interval
     */
    #define MEASURE_INTERVAL_MS(name) IntervalMeasureTime::Tick<false>(# name)

    /**
     *  Measure time between calls in microseconds
     * @param name - unique name of the interval
     */
    #define MEASURE_INTERVAL_US(name) IntervalMeasureTime::Tick<true>(# name)

    /**
     *  Reset specific interval statistics to 0
     * @param name - unique name of the interval
     */
    #define RESET_INTERVAL(name) IntervalMeasureTime::Reset(# name)

    /**
     * Reset statistics of all intervals to 0
     */
    #define RESET_ALL_INTERVALS() IntervalMeasureTime::Reset()

    /**
     *  Get current value of the interval
     * @param name - unique name of the interval
     *
     * @return StatValue<unsigned long> - contains Min, Max, Avg, and Total values
     *         if profile is disabled - return empty StatValue - all values are 0
     */
    #define GET_INTERVAL_VALUE(name) IntervalMeasureTime::GetValue(# name)

    /** Internal helper macro */
    #define DECLARE_SCOPE_COUNTER(name, ...) ScopedCounter<false> PROFILER_TOKENIZE(__counter_, __LINE__)(name, __FILE__, __LINE__,  ## __VA_ARGS__)

    /**
     *  Usage:
     *   {
     *    AUTO_SCOPED_COUNTER;
     *    // Code to measure (will be printed if execution time is greater than 1 ms)
     *   }
     */
    #define AUTO_SCOPED_COUNTER DECLARE_SCOPE_COUNTER(__func__);

    /*
     * Usage:
     *  {
     *    NAME_SCOPED_COUNTER("Name", Threshold);
     *    // Code to measure
     *  }
     */
    #define NAME_SCOPED_COUNTER(name, ...) DECLARE_SCOPE_COUNTER(name, ## __VA_ARGS__);
#else
    #define DECLARE_SCOPE_COUNTER(...)
    #define AUTO_SCOPED_COUNTER
    #define NAME_SCOPED_COUNTER(...)

    #define MEASURE_TIME_MS(...)
    #define MEASURE_TIME_US(...)
    #define MEASURE_TIME(...)

    #define MEASURE_INTERVAL_MS(...)
    #define MEASURE_INTERVAL_US(...)
    #define RESET_INTERVAL(...)
    #define RESET_ALL_INTERVALS()
    #define GET_INTERVAL_VALUE(...) StatValue<unsigned long>()
#endif
