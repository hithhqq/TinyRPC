#pragma once

#include <map>
#include <string>

class MprpcConfig
{
public:
    void LoadConfigFile(const char *config_file);
    std::string Load(const std::string &key); 
    void Trim(std::string &src_buf);
    
private:
    std::map<std::string, std::string> m_configMap;
};