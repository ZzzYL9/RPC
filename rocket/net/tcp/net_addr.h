#ifndef RPC_TCP_NET_ADDR_H
#define RPC_TCP_NET_ADDR_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>


// 这段代码是对网络地址的封装和抽象。
// 它重新封装了网络地址的表示和操作，并提供了一个抽象的接口 `NetAddr`，使得可以针对不同类型的网络地址（比如IPv4、IPv6等）进行统一的处理。
// 这样做的好处是，当需要在程序中使用不同类型的网络地址时，可以方便地替换具体的实现，而无需修改其它代码。
// 因此，这段代码可以被视为网络地址操作的一个抽象层，提供了一种统一的接口来处理不同类型的网络地址。

namespace rocket {

// 网络地址抽象基类
class NetAddr {
public:
    typedef std::shared_ptr<NetAddr> s_ptr;

    // 获取底层套接字地址结构的指针
    virtual sockaddr* getSockAddr() = 0;

    // 获取套接字地址结构的长度
    virtual socklen_t getSockLen() = 0;

    // 获取地址族
    virtual int getFamily() = 0;

    // 将地址转换为字符串表示
    virtual std::string toString() = 0;

    // 检查地址是否有效
    virtual bool checkValid() = 0;
};

// IPV4 网络地址实现
class IPNetAddr : public NetAddr {

public:
    static bool CheckValid(const std::string& addr);
public:
    // 构造函数，通过 IP 地址和端口号初始化
    IPNetAddr(const std::string& ip, uint16_t port);

    // 构造函数，通过 IP 地址初始化
    IPNetAddr(const std::string& addr);

    // 构造函数，通过 sockaddr_in 结构初始化
    IPNetAddr(sockaddr_in addr);

    // 获取底层套接字地址结构的指针
    sockaddr* getSockAddr() override;

    // 获取套接字地址结构的长度
    socklen_t getSockLen() override;

    // 获取地址族
    int getFamily() override;

    // 将地址转换为字符串表示
    std::string toString() override;

    // 检查地址是否有效
    bool checkValid() override;

private:
    std::string m_ip;        // IP地址
    uint16_t m_port {0};     // 端口号
    sockaddr_in m_addr;      // IPV4 套接字地址结构
};


}

#endif