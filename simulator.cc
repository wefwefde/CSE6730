#include <cstddef>
#include <iostream>
#include <map>
#include "simulator.hh"

Simulator::Simulator() {
    this->stopped = false;
	this->now = 0.0;
}

void Simulator::Handle(Event * event) {
	(void) event;
	std::cout << "Simulator is stopped at time " << Simulator::Now() << "\n";
	this->stopped = true;
}

Simulator * Simulator::GetSimulatorPtr() {
	static Simulator simulator;
	
	return &simulator;
}

Time_t Simulator::Now() {
	return GetSimulatorPtr()->now;
}

void Simulator::Schedule(Time_t time, Event * event, Handler * handler) {
    GetSimulatorPtr()->event_map.insert({Simulator::Now() + time, std::make_pair(handler, event)});
}

void Simulator::Run() {
    Simulator * simulator_ptr = GetSimulatorPtr();
	
	while (!simulator_ptr->stopped && !simulator_ptr->event_map.empty())
	{
		simulator_ptr->now = simulator_ptr->event_map.begin()->first;
		std::pair<Handler*, Event*> curr = simulator_ptr->event_map.begin()->second;
		
		simulator_ptr->event_map.erase(simulator_ptr->event_map.begin()); 
		
		Handler* h = curr.first;
		Event* e = curr.second;
		h->Handle(e);
	}
}

void Simulator::StopAt(Time_t time) {
	Simulator::Schedule(time - Simulator::Now(), NULL, GetSimulatorPtr());
}

