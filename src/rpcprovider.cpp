#include "./include/rpcprovider.h"
#include "rpcprovider.h"
#include "mprpcapplication.h"
#include <functional>
#include <google/protobuf/descriptor.h>
#include "rpcheader.pb.h"
#include "zookeeperutil.h"
void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo service_info;
    
    // 获取了服务对象的描述信息
    const google::protobuf::ServiceDescriptor *serviceDesc = service->GetDescriptor();
    // 获取了服务对象的名字
    std::string service_name = serviceDesc->name();
    // 获取了服务对象service的方法的数量
    int methodCnt = serviceDesc->method_count();

    std::cout << "service_name:" << service_name << std::endl;
    for (int i = 0; i < methodCnt; ++i)
    {
        const google::protobuf::MethodDescriptor *methodDesc =  serviceDesc->method(i);
        std::string method_name = methodDesc->name();
        std::cout << "method_name:" << method_name << std::endl; 
        service_info.m_methodMap.insert({method_name, methodDesc});
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}
void RpcProvider::Run()
{
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    server.setThreadNum(4);
    std::cout << "yes" << std::endl;;
    ZKClient zkCli;
    zkCli.start();
    for (auto &sp : m_serviceMap)
    {
        std::string service_path = "/" + sp.first;
        zkCli.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_methodMap)
        {
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", ip.c_str(), port);
            zkCli.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }
    std::cout << "RpcProvider start service at ip:" << ip <<  "port:" << port << std::endl;
    server.start();
    m_eventLoop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        conn->shutdown();
    }
}

// header_size(4个字节) + header_str + args_str
// string中怎么取四个字节？
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp time)
{
    std::string recv_buf = buffer->retrieveAllAsString();

    uint32_t header_size = 0;
    // recv_buf.copy((char*)&header_size, 4, 0);
    memcpy(&header_size, recv_buf.data(), sizeof(header_size));
 
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if (rpcHeader.ParseFromString(rpc_header_str))
    {
        // 数据头反序列成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    }
    else
    {
        std::cout << "rpc_header_str:" << rpc_header_str << "parse error!" << std::endl;
        return;
    }
    std::string arg_str = recv_buf.substr(4 + header_size, args_size);
    std::cout << "header_size:" << header_size << std::endl;
    std::cout << "rpc_header_str:" << rpc_header_str << std::endl;
    std::cout << "header_size:" <<header_size << std::endl;

    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end())
    {
        std::cout << service_name << " is not exit!" << std::endl;
        return;
    }
    google::protobuf::Service *service = it->second.m_service;

    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end())
    {
        std::cout << service_name << ":" << method_name << " is not exit!" << std::endl;
    }

    const google::protobuf::MethodDescriptor *method = mit->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method).New();

    if (!request->ParseFromString(arg_str))
    {
        std::cout << "request parse error! content:" << arg_str << std::endl;
        return;
    }

    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>(this, &RpcProvider::SendRpcResponse, conn, response);

    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        conn->send(response_str);
    }
    else
    {
        std::cout << "serialize response_str error!" << std::endl;
    }
    conn->shutdown();
}
