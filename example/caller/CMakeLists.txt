set(SRC_LIST calluserservice.cpp ../user.pb.cc)
add_executable(consumer ${SRC_LIST})
target_link_libraries(consumer mprpc protobuf absl_log_internal_check_op absl_log_internal_message)