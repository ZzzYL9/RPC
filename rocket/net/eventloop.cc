#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <string.h>
#include "eventloop.h"
#include "common/log.h"
#include "common/util.h"

#define ADD_TO_EPOLL() \
    auto it = m_listen_fds.find(event->getFd());\
    int op = EPOLL_CTL_ADD; \
    if(it != m_listen_fds.end()){ \
        op = EPOLL_CTL_MOD; \
    } \
    epoll_event tmp = event->getEpollEvent(); \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp); \
    if(rt == -1){ \
        ERRORLOG("failed epoll_ctl when add fd %d, errno = %d, error info = %s", event->getFd(), errno, strerror(errno)); \
    } \
    m_listen_fds.insert(event->getFd()); \
    DEBUGLOG("add event success, fd[%d]", event->getFd()); \

// 如果没有找到直接退出
// 否则删除
#define DELETE_TO_EPOLL() \
    auto it = m_listen_fds.find(event->getFd()); \
    if(it == m_listen_fds.end()){  \
        return; \
    } \
    int op = EPOLL_CTL_DEL;  \
    epoll_event tmp = event->getEpollEvent(); \
    int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp); \
    if(rt == -1){ \
        ERRORLOG("failed epoll_ctl when add fd %d, errno = %d, error info = %s", event->getFd(), errno, strerror(errno)); \
    } \
    m_listen_fds.erase(event->getFd()); \
    DEBUGLOG("delete event success, fd[%d]", event->getFd()); \

namespace rocket{

    static thread_local EventLoop* t_current_eventloop = NULL;
    static int g_epoll_max_timeout = 10000;
    static int g_epoll_max_events = 10;


    EventLoop::EventLoop(){
        if(t_current_eventloop != NULL){
            ERRORLOG("failed to create event loop, this thread has create event loop");
            exit(0);
        }
        m_thread_id = getThreadId(); // 获取线程ID

        m_epoll_fd = epoll_create(10);

        if(m_epoll_fd == -1){
            ERRORLOG("failed to create event loop, epoll_create error, error info[%d]", errno);
            exit(0);
        }

        initWakeUpFdEvent();
        initTimer();

        INFOLOG("succ create event loop in thread %d", m_thread_id);
        t_current_eventloop = this;
    }

    EventLoop::~EventLoop(){
        close(m_epoll_fd);
        if(m_wakeup_fd_event){
            delete m_wakeup_fd_event;
            m_wakeup_fd_event = nullptr;
        }
        if(m_timer){
            delete m_timer;
            m_timer = nullptr;
        }
    }

    void EventLoop::loop(){
        m_is_looping = true;
        while(!m_stop_flag) {
            ScopeMutex<Mutex> lock(m_mutex); // 加锁访问资源
            std::queue<std::function<void()>> tmp_tasks;
            m_pending_tasks.swap(tmp_tasks);
            lock.unlock();

            while(!tmp_tasks.empty()){
                std::function<void()> cb = tmp_tasks.front();
                tmp_tasks.pop();
                if(cb) {
                    cb();
                }
            }

            // 如果定时任务需要执行，那么执行
            // 1.怎么判断一个定时任务需要执行？ （now() > TimerEvent.arrive_time)
            // 2.arrive_time 如何让eventloop监听？

            int timeout = g_epoll_max_timeout;
            epoll_event result_events[g_epoll_max_events];
            // DEBUGLOG("now begin to epoll_wait");
            int rt = epoll_wait(m_epoll_fd, result_events, g_epoll_max_events, timeout);
            DEBUGLOG("now end epoll_wait, rt = %d", rt);
            
            if(rt < 0){
                ERRORLOG("epoll_wait error, error=", errno);
            }else{
                for(int i = 0; i < rt; i++){
                    epoll_event trigger_event = result_events[i];
                    FdEvent* fd_event = static_cast<FdEvent*>(trigger_event.data.ptr); // ????????????
                    if(fd_event == nullptr){ // ???????????????
                        continue;
                    }
                    if(trigger_event.events & EPOLLIN){
                        DEBUGLOG("fd %d trigger in EPOLLIN event", fd_event->getFd());
                        addTask(fd_event->handler(FdEvent::IN_EVENT));
                    }
                    if(trigger_event.events & EPOLLOUT){  //为什么不能写与
                        DEBUGLOG("fd %d trigger in EPOLLOUT event", fd_event->getFd());
                        addTask(fd_event->handler(FdEvent::OUT_EVENT));
                    }
                }
            }

        }
    }

    void EventLoop::wakeup(){
        m_wakeup_fd_event->wakeup();
    }

    void EventLoop::stop(){}

    void EventLoop::addEpollEvent(FdEvent *event){
        if(isInLoopThread()){
            ADD_TO_EPOLL();
            // auto it = m_listen_fds.find(event->getFd());
            // int op = EPOLL_CTL_ADD;
            // if(it != m_listen_fds.end()){
            //     op = EPOLL_CTL_MOD;
            // }
            // epoll_event tmp = event->getEpollEvent();
            // int rt = epoll_ctl(m_epoll_fd, op, event->getFd(), &tmp);
            // if(rt == 0){
            //     ERRORLOG("failed epoll_ctl when add fd %d, errno = %d, error info = %s", event->getFd(), errno, strerror(errno));
            // }
        }else{
            auto cb = [this, event]() {
                ADD_TO_EPOLL();
            };
            addTask(cb, true); // ??????????????
        }
    }

    void EventLoop::deleteEpollEvent(FdEvent *event){
        if(isInLoopThread()){
            DELETE_TO_EPOLL();
        }else{
            auto cb = [this, event](){
                DELETE_TO_EPOLL();
            };
            addTask(cb, true);
        }
    }

    bool EventLoop::isInLoopThread(){
        return getThreadId() == m_thread_id;
    }

    void EventLoop::addTask(std::function<void()> cb, bool is_wake_up /*==false*/){
        ScopeMutex<Mutex> lock(m_mutex);
        m_pending_tasks.push(cb);
        lock.unlock();
        if(is_wake_up){
            wakeup();
        }
    }

    void EventLoop::dealWakeup(){

    }

    void EventLoop::initTimer(){
        m_timer = new Timer();
        addEpollEvent(m_timer);
    }

    void EventLoop::addTimerEvent(TimerEvent::s_ptr event){
        m_timer->addTimerEvent(event);
    }

    void EventLoop::initWakeUpFdEvent(){
        m_wakeup_fd = eventfd(0, EFD_NONBLOCK); 
        if(m_wakeup_fd < 0){
            ERRORLOG("failed to create event loop, epollfd create error, error info[%d]", errno);
            exit(0);
        }

        m_wakeup_fd_event = new WakeupFdEvent(m_wakeup_fd);
        
        m_wakeup_fd_event->listen(FdEvent::IN_EVENT, [this](){
        char buf[8];
        while(read(m_wakeup_fd, buf, 8) != -1 && errno != EAGAIN){
        }
        DEBUGLOG("read full bytes from wakeup fd[%d]", m_wakeup_fd);
        });

        addEpollEvent(m_wakeup_fd_event);
    }

    EventLoop* EventLoop::GetCurrentEventLoop(){
        if(t_current_eventloop){
            return t_current_eventloop;
        }
        t_current_eventloop = new EventLoop();
        return t_current_eventloop;
    }

    bool EventLoop::isLooping(){
        return m_is_looping;
    }
}
