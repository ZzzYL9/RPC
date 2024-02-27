#ifndef PRC_TIMER_H
#define PRC_TIMER_H

#include <map>
#include "common/mutex.h"
#include "common/util.h"
#include "fdevent.h"
#include "timer_event.h"

namespace rocket{

class Timer : public FdEvent{
    public:
        Timer();
        ~Timer();

        void addTimerEvent(TimerEvent::s_ptr event);

        void deleteTimerEvent(TimerEvent::s_ptr event);

        void onTimer(); //当发生IO事件后，eventloop会执行这个回调函数
        
        void resetArriveTime();
    private:
        std::multimap<int64_t, TimerEvent::s_ptr> m_pending_events;

        Mutex m_mutex;
};

}

#endif