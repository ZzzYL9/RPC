#ifndef RPC_WAKE_UP_FDEVENT_H
#define RPC_WAKE_UP_FDEVENT_H

#include "fdevent.h"

namespace rocket{

class WakeupFdEvent : public FdEvent{

public:
    WakeupFdEvent(int fd);
    ~WakeupFdEvent();

    // void init();
    void wakeup();

private:

};

}

#endif