#include "tcp_server.h"
#include "../eventloop.h"
// #include "rocket/net/tcp/tcp_connection.h"
#include "../../common/log.h"
#include "../../common/config.h"


// 这段代码实现了一个 TCP 服务器类 `TcpServer`，其中包含了以下功能：

// - 构造函数用于初始化服务器，并调用 `init()` 函数进行初始化。
// - 析构函数用于释放资源。
// - `init()` 函数用于初始化服务器，包括创建监听器、获取事件循环、创建 IO 线程组、创建监听套接字的事件处理器以及添加事件到主事件循环中。
// - `onAccept()` 函数为接受连接的回调函数，接受客户端连接，并创建 TCP 连接对象，然后将连接对象添加到客户端连接集合中。
// - `start()` 函数用于启动服务器，先启动 IO 线程组，然后运行主事件循环。
// - `ClearClientTimerFunc()` 函数为清理客户端连接的定时器回调函数，遍历客户端连接集合，将处于关闭状态的连接从集合中删除。

namespace rocket {

TcpServer::TcpServer(NetAddr::s_ptr local_addr) : m_local_addr(local_addr) {

    init(); 

    INFOLOG("rocket TcpServer listen sucess on [%s]", m_local_addr->toString().c_str());
}

TcpServer::~TcpServer() {
    if (m_main_event_loop) {
        delete m_main_event_loop;
        m_main_event_loop = NULL;
    }
    if (m_io_thread_group) {
        delete m_io_thread_group;
        m_io_thread_group = NULL; 
    }
    if (m_listen_fd_event) {
        delete m_listen_fd_event;
        m_listen_fd_event = NULL;
    }
}


void TcpServer::init() {

    // 创建监听器
    m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);

    // 获取主事件循环
    m_main_event_loop = EventLoop::GetCurrentEventLoop();
    // 创建 IO 线程组
    // m_io_thread_group = new IOThreadGroup(Config::GetGlobalConfig()->m_io_threads);
    m_io_thread_group = new IOThreadGroup(2);

    // 创建监听套接字的事件处理器
    m_listen_fd_event = new FdEvent(m_acceptor->getListenFd());
    // 注册监听事件，并绑定回调函数
    m_listen_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpServer::onAccept, this));
    // 将监听套接字事件添加到主事件循环中
    m_main_event_loop->addEpollEvent(m_listen_fd_event);

    // 创建清理客户端定时器事件
    // m_clear_client_timer_event = std::make_shared<TimerEvent>(5000, true, std::bind(&TcpServer::ClearClientTimerFunc, this));
    // 将清理客户端定时器事件添加到主事件循环中
    // m_main_event_loop->addTimerEvent(m_clear_client_timer_event);

}


void TcpServer::onAccept() {
    // 接受客户端连接
    auto re = m_acceptor->accept();
    int client_fd = re.first;
    // NetAddr::s_ptr peer_addr = re.second;
    // 客户端连接计数加一
    m_client_counts++;

    // 把 cleintfd 添加到任意 IO 线程里面
    // 获取一个 IO 线程
    // IOThread* io_thread = m_io_thread_group->getIOThread();
    // 创建 TCP 连接对象
    // TcpConnection::s_ptr connetion = std::make_shared<TcpConnection>(io_thread->getEventLoop(), client_fd, 128, peer_addr, m_local_addr);
    // connetion->setState(Connected);

    // 将连接对象添加到客户端连接集合中
    // m_client.insert(connetion);

    INFOLOG("TcpServer succ get client, fd=%d", client_fd);
}

void TcpServer::start() {
    m_io_thread_group->start(); // 启动 IO 线程组
    m_main_event_loop->loop(); // 运行主事件循环
}

// 清理客户端连接的定时器回调函数
// void TcpServer::ClearClientTimerFunc() {
//     auto it = m_client.begin();
//     // 遍历客户端连接集合
//     for (it = m_client.begin(); it != m_client.end(); ) {
//         // TcpConnection::ptr s_conn = i.second;
//             // DebugLog << "state = " << s_conn->getState();
//         // 检查连接是否有效并且处于关闭状态，若是，则从集合中删除
//         if ((*it) != nullptr && (*it).use_count() > 0 && (*it)->getState() == Closed) {
//             // need to delete TcpConnection
//             DEBUGLOG("TcpConection [fd:%d] will delete, state=%d", (*it)->getFd(), (*it)->getState());
//             it = m_client.erase(it);
//         } else {
//             it++;
//         }
//     }
// }

}