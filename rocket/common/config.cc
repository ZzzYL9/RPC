//
// Created by zyl on 2024/1/11.
//

#include "config.h"
#include <tinyxml/tinyxml.h>

#define READ_XML_NODE(name, parent) \
TiXmlElement* name##_node = parent->FirstChildElement(#name); \
if(!name##_node){ \
    printf("Start rocket server error, failed to read node [%s]\n", #name); \
    exit(0); \
} \

#define READ_STR_FROM_XML_NODE(name, parent) \
    TiXmlElement* name##_node = parent->FirstChildElement(#name); \
    if(!name##_node || !name##_node->GetText()){ \
        printf("Start rocket server error, failed to read config file %s\n", #name); \
        exit(0); \
    } \
    std::string name##_str = std::string(name##_node->GetText()); \

namespace rocket{
    static Config* g_config = nullptr;

    Config* Config::GetGlobalConfig() {
        return g_config;
    }
    // 单例模式
    void Config::SetGlobalConfig(const char *xmlfile) {
        if(g_config == nullptr) {
            g_config = new Config(xmlfile);
        }
    }

    Config::Config(const char *xmlfile) {
        TiXmlDocument* xml_document = new TiXmlDocument();

        bool rt = xml_document->LoadFile(xmlfile);
        if(!rt){
            printf("Start rocket server error, failed to read config file %s\n", xmlfile);
            exit(0);
        }

        READ_XML_NODE(root, xml_document);
        READ_XML_NODE(log, root_node);

        READ_STR_FROM_XML_NODE(log_level, log_node);
        // 下面这段代码被定义为宏：READ_STR_FROM_XML_NODE
        // TiXmlElement* log_level_node = log_node->FirstChildElement("log_level");
        // if(!log_level_node || !log_level_node->GetText()){
        //     printf("Start rocket server error, failed to read config file %s\n", "log_level");
        //     exit(0);
        // }
        // std::string log_level = std::string(log_level_node->GetText());

        m_log_level = log_level_str;
    }
}