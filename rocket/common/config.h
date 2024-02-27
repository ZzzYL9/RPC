//
// Created by zyl on 2024/1/10.
//

#ifndef RPC_CONFIG_H
#define RPC_CONFIG_H

#include <map>
#include <tinyxml/tinyxml.h>

namespace rocket{
    class Config{
    public:

        Config(const char* xmlfile);

    public:
        static Config* GetGlobalConfig();
        static void SetGlobalConfig(const char* xmlfile);

    public:
        std::string m_log_level;
    };
}

#endif //RPC_CONFIG_H
