# set(SRC_LIST userservice.cpp ../user.pb.cc)
set(SRC_LIST friendservice.cpp ../frend.pb.cc)
add_executable(provider ${SRC_LIST})

target_link_libraries(provider mprpc protobuf absl_log_internal_check_op absl_log_internal_message)