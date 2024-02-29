#ifndef RPC_TCP_CLIENT_H
#define RPC_TCP_CLIENT_H

#include "net_addr.h"
#include "eventloop.h"
#include "tcp_connection.h"
#include "../abstact_protocol.h"

namespace rocket{

class TcpClient{
    public:
        TcpClient(NetAddr::s_ptr peer_addr);

        ~TcpClient();

        // 异步的进行connect
        // 如果connect成功，则done会被执行
        void connect(std::function<void()> done);

        // 异步的发送Message
        // 如果读发送message成功， 会调用done函数， 函数的入参就是message对象
        void writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

        // 异步的读取message
        // 如果读取message成功， 会调用done函数， 函数的入参就是message对象
        void readMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

    private:
        NetAddr::s_ptr m_peer_addr;
        EventLoop* m_event_loop{nullptr};

        int m_fd {-1};
        FdEvent* m_fd_event{nullptr};

        TcpConnection::s_ptr m_connection;

};

}

#endif