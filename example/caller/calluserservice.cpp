#include <iostream>
#include "mprpcapplication.h"
#include "../user.pb.h"

int main(int argc, char *argv[])
{
    MprpcApplication::Init(argc, argv);

    // 演示调用远程发布的rpc方法Login
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr);
    // MprpcController controller;
    if (0 == response.result().errcode())
    {
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }
    fixbug::RegisterRequest req;
    req.set_id(888);
    req.set_name("yeye");
    req.set_pwd("1222");
    fixbug::RegisterResponse rep;
    stub.Register(nullptr, &req, &rep, nullptr);
    return 0;
}   