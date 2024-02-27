#ifndef RPC_EVENTLOOP_H
#define RPC_EVENTLOOP_H

#include <pthread.h>
#include <set>
#include <functional>
#include <queue>
#include "common/mutex.h"
#include "net/fdevent.h"
#include "net/wakeup_fd_event.h"
#include "net/timer.h"

namespace rocket{
class EventLoop{
public:
    EventLoop();

    ~EventLoop();

    void loop();

    void wakeup();

    void stop();

    void addEpollEvent(FdEvent* event);

    void deleteEpollEvent(FdEvent* event);

    bool isInLoopThread(); // 判断是否是当前线程添加event

    void addTask(std::function<void()> cb, bool is_wake_up = false); // ??? 把任务添加到pending队列中等待线程执行完上一个任务后再来执行

    void addTimerEvent(TimerEvent::s_ptr event);

private:
    void dealWakeup();

    void initWakeUpFdEvent();
    
    void initTimer();

private:
    pid_t m_thread_id = 0;
    
    int m_epoll_fd = 0;

    int m_wakeup_fd = 0;

    WakeupFdEvent* m_wakeup_fd_event = nullptr;

    bool m_stop_flag = false;

    std::set<int> m_listen_fds;

    std::queue<std::function<void()>> m_pending_tasks;
    
    Mutex m_mutex;

    Timer* m_timer{NULL};
};
}

#endif