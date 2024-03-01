#ifndef RPC_ABSTRACT_PROTOCOL_H
#define RPC_ABSTRACT_PROTOCOL_H

#include <memory>


namespace rocket {

class AbstractProtocol {
    public:
        typedef std::shared_ptr<AbstractProtocol> s_ptr;

    virtual ~AbstractProtocol() {}

    public:
        std::string m_msg_id;     // 请求号，唯一标识一个请求或者响应
};

}


#endif