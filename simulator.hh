#ifndef SIMULATOR_HH
#define SUMULATOR_HH
#include "event.hh"
#include "handler.hh"

class Simulator : public Handler {
public:
    Simulator(Simulator const & simulator) = delete;

    void Handle(Event* event) override;
    static Simulator * GetSimulatorPtr();
    static Time_t Now();
    static void Schedule(Time_t time, Event * event, Handler * handler);
    static void Run();
    static void StopAt(Time_t t);

private:
    Simulator();
    std::map<Time_t, std::pair<Handler*, Event*>> event_map;
    bool stopped;
    Time_t now;
};
#endif