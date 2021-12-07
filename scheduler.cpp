
#include <cstdint>
#include <map>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>

using Time = std::uint64_t; ///! nanos since epoch

/** Base class for all events to be handled by the scheduler */
struct Event
{
    virtual void fire(Time scheduled, Time now) = 0;
};

/** A class that functions as a container for events to be fired in the future.
    The same event can be added multiple times into the scheduler.
    Distinct events can be added at the same time.
 */
class Scheduler
{
public:
    virtual ~Scheduler() = default;
    virtual void schedule(Event *ev, Time tm) = 0; ///! Schedules one event
    virtual bool check(Time t) = 0;                ///! Checks events and fires accordingly. "t" is guaranteed ascending
};

/** A scheduler implementation that uses a trivial solution with the STL.
    This solution is inappropriate because:
    1. Average complexity of schedule() is O(logN)
    However it meets the requirements on:
    2. Average complexity of check() is O(1)
 */
class StandardScheduler : public Scheduler
{
public:
    StandardScheduler();
    virtual void schedule(Event *ev, Time tm) override;
    virtual bool check(Time t) override;

private:
    using EventMap = std::multimap<Time, Event *>;
    EventMap events;
    Time current;
};

StandardScheduler::StandardScheduler() : current(0) {}

void StandardScheduler::schedule(Event *ev, Time t)
{
    events.insert({t, ev});
}

bool StandardScheduler::check(Time t)
{
    if (t > current)
    {
        current = t;
    }
    EventMap::iterator it;
    for (it = events.begin(); it != events.end(); it++)
    {
        Time tm = it->first;
        Event *ev = it->second;
        if (tm > current)
        {
            break;
        }
        ev->fire(tm, current);
    }
    if (it != events.begin())
    {
        events.erase(events.begin(), it);
        return true;
    }
    return false;
}

struct Notifier
{
    uint32_t counter = 0;
};

struct SimpleEvent : public Event
{
    Notifier *notify;
    SimpleEvent(Notifier *n) : notify(n) {}
    virtual void fire(Time scheduled, Time now) override
    {
        notify->counter++;
    }
};

static inline std::uint64_t ticks()
{
    return __builtin_ia32_rdtsc();
}

bool testScheduler(Scheduler &sch, std::uint64_t numsamples)
{
    Notifier notify;
    std::vector<SimpleEvent> events(numsamples, &notify);
    std::vector<Time> times(numsamples);

    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<std::uint64_t> dist(0, 10 * numsamples);
    for (std::uint64_t &tm : times)
        tm = dist(g);

    std::uint64_t t0 = ticks();
    for (std::uint32_t j = 0; j < numsamples; ++j)
    {
        sch.schedule(&events[j], times[j]);
    }
    std::uint64_t t1 = ticks();
    for (Time now = 0; now < numsamples * 10; now += 5)
    {
        sch.check(now);
    }
    std::uint64_t t2 = ticks();
    std::cout << "Timings schedule:" << (t1 - t0) / numsamples << " check:" << (t2 - t1) / numsamples << std::endl;
    if (notify.counter != numsamples)
    {
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cout << "Usage:\n\t" << argv[0] << " <numsamples>" << std::endl;
        return 0;
    }
    std::uint64_t numsamples = ::atoll(argv[1]);

    StandardScheduler sch;
    if (!testScheduler(sch, numsamples))
    {
        std::cout << "Failed!" << std::endl;
        return 1;
    }
    std::cout << "Success!" << std::endl;
    return 0;
}
