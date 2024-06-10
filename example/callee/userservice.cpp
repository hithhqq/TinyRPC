#include <iostream>
#include <string>
#include "../user.pb.h"
// #include "../../src/include/mprpcapplication.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
class UserService : public fixbug::UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "doing local service: Login" << std::endl;
        std::cout << "name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }
    bool Register(uint32_t id, std::string name, std::string pwd)
    {
        std::cout << "doing local service Register" << std::endl;
        std::cout << "id: " << id << " name: " << name << " pwd: " << pwd << std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
                        const ::fixbug::LoginRequest* request,
                        ::fixbug::LoginResponse* response,
                        ::google::protobuf::Closure* done)
    {
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool login_request = Login(name, pwd);

        // 把响应写入，包括错误码， 错误消息，返回值
        fixbug::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_request);

        done->Run();
    }
    void Register(::google::protobuf::RpcController* controller,
                        const ::fixbug::RegisterRequest* request,
                        ::fixbug::RegisterResponse* response,
                        ::google::protobuf::Closure* done)
    {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();
        bool res = Register(id, name, pwd);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(res);

        done->Run();
    }

};

int main(int argc, char *argv[])
{
    MprpcApplication& mrpc = MprpcApplication::GetInstance();
    mrpc.Init(argc, argv);
    RpcProvider provider;
    provider.NotifyService(new UserService());
    provider.Run();
    return 0;
}