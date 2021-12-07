
#include <cstdint>
#include <map>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>

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
    uint64_t counter = 0;
    Time last = 0;
    bool error = false;
};

struct SimpleEvent : public Event
{
    Notifier *notify;
    SimpleEvent(Notifier *n) : notify(n) {}
    virtual void fire(Time scheduled, Time now) override
    {
        notify->counter++;
        if ((now < scheduled) || (now < notify->last))
        {
            notify->error = true;
        }
        notify->last = now;
    }
};

bool testScheduler(Scheduler &sch, std::uint64_t numsamples, std::uint32_t numreposts)
{
    Notifier notify;
    std::vector<SimpleEvent> events(numsamples, &notify);

    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<std::uint64_t> dist(0, 10 * numsamples);

    using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;
    Timestamp t0 = std::chrono::steady_clock::now();
    for (std::uint32_t j = 0; j < numsamples; ++j)
    {
        for (std::uint32_t k = 0; k < numreposts; ++k)
        {
            Time scheduled = dist(g);
            sch.schedule(&events[j], scheduled);
        }
    }
    Timestamp t1 = std::chrono::steady_clock::now();
    for (Time now = 0; now <= numsamples * 10; now += 5)
    {
        sch.check(now);
    }
    Timestamp t2 = std::chrono::steady_clock::now();

    std::cout << "Timings schedule:" << std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count() / (numsamples * numreposts)
              << " check:" << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() / (numsamples * numreposts) << std::endl;
    if (notify.error || (notify.counter != numsamples * numreposts))
    {
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage:\n\t" << argv[0] << " <numsamples> <numreposts>" << std::endl;
        return 0;
    }
    std::uint64_t numsamples = ::atoll(argv[1]);
    std::uint32_t numreposts = ::atoll(argv[2]);

    StandardScheduler sch;
    if (!testScheduler(sch, numsamples, numreposts))
    {
        std::cout << "Failed!" << std::endl;
        return 1;
    }
    std::cout << "Success!" << std::endl;
    return 0;
}
