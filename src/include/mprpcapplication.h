#pragma once
#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "rpcprovider.h"
class MprpcApplication
{
public:
    static void Init(int argc, char **argv);
    static MprpcApplication& GetInstance()
    {
        static MprpcApplication mprpc;
        return mprpc;
    }
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig m_config;
    MprpcApplication() = default;
    MprpcApplication(const MprpcApplication &) = delete;
};