#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include "../../common/log.h"
#include "net_addr.h"
#include "tcp_acceptor.h"


namespace rocket {

// TcpAcceptor 类的构造函数
TcpAcceptor::TcpAcceptor(NetAddr::s_ptr local_addr) : m_local_addr(local_addr) {
    // 检查本地地址是否有效
    if (!local_addr->checkValid()) {
        ERRORLOG("invalid local addr %s", local_addr->toString().c_str());
        exit(0);
    }

    // 获取本地地址的地址族
    m_family = m_local_addr->getFamily();
    
    // 创建监听套接字
    m_listenfd = socket(m_family, SOCK_STREAM, 0);

    // 检查监听套接字是否创建成功
    if (m_listenfd < 0) {
        ERRORLOG("invalid listenfd %d", m_listenfd);
        exit(0);
    }

    // 设置套接字选项，允许地址重用
    int val = 1;
    if (setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0) {
        ERRORLOG("setsockopt REUSEADDR error, errno=%d, error=%s", errno, strerror(errno));
    }

    // 将监听套接字绑定到本地地址
    socklen_t len = m_local_addr->getSockLen();
    if(bind(m_listenfd, m_local_addr->getSockAddr(), len) != 0) {
        ERRORLOG("bind error, errno=%d, error=%s", errno, strerror(errno));
        exit(0);
    }

    // 开始监听连接
    if(listen(m_listenfd, 1000) != 0) {
        ERRORLOG("listen error, errno=%d, error=%s", errno, strerror(errno));
        exit(0);
    }
}

// TcpAcceptor 类的析构函数
TcpAcceptor::~TcpAcceptor() {
}

// 获取监听套接字的文件描述符
int TcpAcceptor::getListenFd() {
    return m_listenfd;
}

// 接受客户端连接
std::pair<int, NetAddr::s_ptr> TcpAcceptor::accept() {
    // 如果是 IPv4 地址族
    if (m_family == AF_INET) {
        sockaddr_in client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t clien_addr_len = sizeof(clien_addr_len);

        // 接受客户端连接
        int client_fd = ::accept(m_listenfd, reinterpret_cast<sockaddr*>(&client_addr), &clien_addr_len);
        if (client_fd < 0) {
        ERRORLOG("accept error, errno=%d, error=%s", errno, strerror(errno));
        }
        // 创建客户端地址对象
        IPNetAddr::s_ptr peer_addr = std::make_shared<IPNetAddr>(client_addr);
        // 打印客户端地址信息
        INFOLOG("A client have accpeted succ, peer addr [%s]", peer_addr->toString().c_str());

        // 返回客户端文件描述符和客户端地址对象
        return std::make_pair(client_fd, peer_addr);
    } else {
        // 其他地址族暂未实现
        return std::make_pair(-1, nullptr);
    }
}

}