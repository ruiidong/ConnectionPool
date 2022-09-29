cmake_minimum_required (VERSION 3.0)
project(CONNECTIONPOOL)

# 配置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

aux_source_directory(./ SRC_LIST)


add_executable(main ${SRC_LIST})
target_link_libraries(main mysqlclient)