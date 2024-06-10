# aux_source_directory(. SRC_LIST)
set (SRC_LIST 
    mprpcapplication.cpp 
    mprpcconfig.cpp 
    rpcheader.pb.cc 
    rpcprovider.cpp 
    mprpcchannel.cpp
    Logger.cpp
    zookeeperutil.cpp)
add_library(mprpc ${SRC_LIST})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DTHREADED")
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)