#include "mprpcchannel.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/message.h"
#include "rpcheader.pb.h"
#include "mprpcapplication.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method, google::protobuf::RpcController *controller, const google::protobuf::Message *request, google::protobuf::Message *response, google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();
    uint32_t arg_size = 0;
    std::string args_str;
    if (request->SerializeToString(&args_str))
    {
        arg_size = args_str.size();
    }
    else{
        controller->SetFailed("serialize request error!");
        return;
    }
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(arg_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;

    if (rpcHeader.SerializeToString(&rpc_header_str))
    {
        header_size = rpc_header_str.size(); 
    }
    else
    {
        controller->SetFailed("serialize rpc header error!");
        return;
    }

    char header_bytes[4];
    memcpy(header_bytes, &header_size, sizeof(header_bytes));

    std::string send_rpc_str(header_bytes, sizeof(header_bytes));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_str: " << args_str << std::endl;  

    int sock_fd = 0;
    sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd == -1)
    {
        controller->SetFailed("create socket error! errno: ");
        exit(EXIT_FAILURE);
    }
    std::string ip = MprpcApplication::GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetConfig().Load("rpcserverport").c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        controller->SetFailed("");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    // 发送rpc请求
    if (send(sock_fd, send_rpc_str.c_str(), send_rpc_str.size(), 0) == -1)
    {
        std::cout << "send error! errno: " << errno << std::endl;
        close(sock_fd);
        return;
    }

    // 接收rpc请求的响应值
    char recv_buf[1024] = {0}; 
    int recv_size = recv(sock_fd, recv_buf, 1024, 0);
    if (recv_size == -1)
    {
        close(sock_fd);
        std::cout << "recv error! errno: " << errno << std::endl;
        return;
    }   

    // std::string response_str(recv_buf, 0, recv_size);
    if(!response->ParseFromArray(recv_buf, recv_size))
    {
        std::cout << "parse error! response_str:" << recv_buf << std::endl;
        close(sock_fd);
        return;
    }
    close(sock_fd);
}