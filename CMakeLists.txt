# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.23)
# 项目信息
PROJECT (Guess_num_socket)
# 指定生成目标
ADD_EXECUTABLE(client client_.c)
ADD_EXECUTABLE(server server_.c)