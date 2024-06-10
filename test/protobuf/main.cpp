#include "test.pb.h"
#include <iostream>
#include <string>
#include <string>
int main()
{
    // 生成一个请求
    fixbug::LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    std::string send_str;
    // 将请求序列化
    if (req.SerializeToString(&send_str)) 
    {
        std::cout << send_str << std::endl;
    }

    // 将请求反序列化
    fixbug::LoginRequest reqB;
    if (reqB.ParseFromString(send_str))
    {
        std::cout << reqB.name() << std::endl;
        std::cout << reqB.pwd() << std::endl;
    }

    return 0;
}