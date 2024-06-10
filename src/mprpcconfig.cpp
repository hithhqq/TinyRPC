#include "mprpcconfig.h"

#include <iostream>
#include <string>


void MprpcConfig::LoadConfigFile(const char *config_file)
{
    FILE *pf = fopen(config_file, "r");
    if (nullptr == pf)
    {
        std::cout << config_file << "is invalid!" << std::endl;
    }

    while(!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        // 去掉字符串前面多余的空格
        std::string read_buf(buf);
        Trim(read_buf);
        if (read_buf[0] == '#' || read_buf.empty())
        {
            continue;
        }
        int idx = read_buf.find('=');
        if (idx == -1)
        {
            continue;
        }
        std::string key;
        std::string value;
        key = read_buf.substr(0, idx);
        Trim(key);
        int endidx = read_buf.find('\n', idx);
        read_buf[endidx] = '\0';
        value = read_buf.substr(idx + 1, endidx);
        Trim(value);
        m_configMap.insert(std::make_pair(key, value));
    }
}

std::string MprpcConfig::Load(const std::string &key)
{
   auto it = m_configMap.find(key);
   if (it == m_configMap.end())
   {
        return "";
   }
   return it->second;
}

void MprpcConfig::Trim(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        src_buf = src_buf.substr(idx, src_buf.size() - idx);
    }
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        src_buf = src_buf.substr(0, idx + 1);
    }
}
