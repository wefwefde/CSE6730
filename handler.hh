#ifndef HANDLER_HH
#define HANDLER_HH
#include "event.hh"

class Handler {
public:
    virtual void Handle(Event* event) = 0;
};
#endif