#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <bitset>
#include <algorithm>

using namespace std;
class Event;
typedef double Time_t;
typedef map<Time_t, Event*> EventList_t;
typedef EventList_t::value_type EventPair_t;

class Handler {
public:
    virtual void Handle(Event* event) = 0;
};
class Packet;
class Event {
    Time_t time;
    Handler* handler;
    Packet* packet;
public:
    Event(Time_t time_t, Handler* handler_t, Packet* packet_t){
    	time = time_t;
    	handler = handler_t;
    	packet = packet_t;
    }
    Time_t GetTime() const;
    Handler* GetHandler() const;
    Packet* GetPacket() const;
};

class Point : public Handler {
public:
    Point(int input_id){
    	ownId = input_id
    }
    virtual void Handle(Event* event) override = 0;
    void AddNeighbor(Point* neighbor, int neighbour_id);
    void AddLink(Point* neighbor, Link* link);
    int GetIndex(int* neighbour_id);
    bool IsLinkOccupied(int* neighbour_id);
    int ownId;
protected:
    vector<int*> id;
    map<int*,int> idIndex;
    vector<Point*> neighbors;
    vector<Link*> links;
    map<Point*, queue<Packet*>> queues;
    vector<int*> occupiedStatus;
};

class EndPoint : public Point {
public:
    EndPoint();
    void Handle(Event* event) override;
    void SendPacket(Point* neighbor, Packet* packet);
};

class Switch : public Point {
public:
    Switch();
    void Handle(Event* event) override;
    void AddForwardingEntry(const bitset<32>& destination, Point* nextHop);
    void ForwardPacket(Packet* packet);
};

class Link : public Handler {
public:
    Link(Point* owner1, Point* owner2){
    	pointOwner1 = owner1;
    	pointOwner2 = owner2;
    	srand(static_cast<unsigned>(time(0)));
    	bandwidth = 10.0 + static_cast<double>(rand()) / (RAND_MAX / (20.0 - 10.0));
    	latency = 2.0 + static_cast<double>(rand()) / (RAND_MAX / (5.0 - 2.0));
    }
    Point* pointOwner1;
    Point* pointOwner2;
    void Handle(Time_t t) override;
    double GetBandwidth() const;
    double GetLatency() const;
    //void OccupyLink(Point* occupant);
    //void ReleaseLink();
private:
    double bandwidth;
    double latency;
    //Point* occupiedBy; 
};

class Packet {
public:
    Packet(Point* start, Point* dest, double flowSize, int flowId, int routingMethod){
    	startPoint = start;
    	destination = dest;
    	packetSize = flowSize;
    	id = flowId;
    	method = routingMethod;
    //Time_t transferTime;
    }
public:
    Point* startPoint;
    Point* destination;
    double packetSize;
    int id= flowId;
    int method;
};
class Simulator : public Handler {
public:
    Simulator();
    void Handle(Time_t t) override;
    static void Schedule(Time_t t, Event* e);
    static void Run();
    static void StopAt(Time_t t);
    static Time_t Now();              // Return current simulation tiome
private:
    static EventList_t events;
    static bool stopped;
    static Time_t now;
};

EventList_t Simulator::events;
bool        Simulator::stopped = false;
Time_t      Simulator::now = 0;
Simulator*  Simulator::instance = 0;

Simulator::Simulator() {
    instance = this;
}

void Simulator::Handle(Time_t t)
{
  cout << "Simulator handling stop event at time " << t << "; " << events.size() << " events remaining in queue" << endl;
  stopped = true;
}

void Simulator::Schedule(Time_t t, Event* e) {
    events.insert(EventPair_t(now + t, e));
}

void Simulator::Run()
{
  while (!stopped && !events.empty())
    {
      EventPair_t currentEvent = *events.begin();
      events.erase(events.begin());
      now = currentEvent.first;  
      Event* e = currentEvent.second;
      Handler* h = e::GetHandler()
      h->Handle(e);
    }
}


void Simulator::StopAt(Time_t t)
{
  Simulator::Schedule(t - Simulator::Now(), Simulator::instance);
}

void Point::AddLink(Point* neighbor, Link* link) {
    links.push_back(link);
    queues[neighbor] = queue<Packet*>();
    //occupiedStatus.push_back(false); 
}

void Point::AddNeighbor(Point* neighbor, int neighbour_id) {
    neighbors.push_back(neighbor);
    id.push_back(new int(neighbour_id));
    occupiedStatus.push_back(new int(0));
    idIndex[id.back()] = id.size() - 1;
}

bool Point::IsLinkOccupied(int neighbour_id) {
    size_t neighborIndex = GetIndex(&neighbour_id);
    return neighborIndex < occupiedStatus.size() ? occupiedStatus[neighborIndex] : false;
}

int Point::GetIndex(int* neighbour_id){
    auto it = idIndex.find(neighbour_id);
    if (it != idIndex.end()) {
        size_t neighborIndex = it->second;
        return neighborIndex;
    } else {
        return false;
    }
}
}

void EndPoint::SendPacket(Point* neighbor, Packet* packet) {
    if (IsLinkOccupied(neighbor)) {
        queues[neighbor].push(packet);
    } else {
    	int index = 
        Simulator::Schedule(Simulator::Now()+, this);
        // Set the link as occupied]
}
}
void Switch::AddForwardingEntry(const bitset<32>& destination, Point* nextHop) {
    forwardingTable[destination] = nextHop;
}

void Switch::ForwardPacket(Packet* packet) {
    // Perform longest prefix match and forward the packet to the next hop
    bitset<32> destinationAddr;
    // Extract destination address from packet (assuming it's a bitset<32>)
    destinationAddr = packet->GetDestination();
    auto entry = forwardingTable.find(destinationAddr);
    if (entry != forwardingTable.end()) {
        // Forward the packet to the next hop
        entry->second->SendPacket(this, packet);
    } else {
        // Drop the packet if no forwarding entry is found
        // (You may want to add logging or handle this differently)
        delete packet; // Assuming ownership is transferred to the receiving point
    }
}

void EndPoint::Handle(Event* e) {
    Event* packetEvent = dynamic_cast<Event*>(e);
    if (packetEvent) {
        Packet* packet = packetEvent->GetPacket();
        if (!neighbors.empty()) {
            Point* firstNeighbor = neighbors.front();
            SendPacket(firstNeighbor, packet);
        } else {
            cout << "Endpoint has no neighbors to send the packet to." << endl;
        }
    } else {
        cout << "Invalid event type for Endpoint." << endl;
    }
}

void Switch::Handle(Event* e) {
    Event* packetEvent = dynamic_cast<Event*>(e);
    if (packetEvent) {
        Packet* packet = packetEvent->GetPacket();
        ForwardPacket(packet);
    } else {
        std::cout << "Invalid event type for Switch." << std::endl;
    }
}

Time_t Event::GetTime() const{
    return time
}
Handler* Event::GetHandler() const{
    return handler
}
Packet* Event::GetPacket() const {
    return packet;
}
