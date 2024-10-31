cmake_minimum_required(VERSION 3.0)
project(mymuduo)

# libmymuo.so 输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 设置调试信息
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 定义参与编译的源代码文件
aux_source_directory(. SRC_LIST)

# 头文件目录
include_directories(${PROJECT_SOURCE_DIR}/src/include)

# 编译 libmymuo.so
add_library(mymuduo SHARED ${SRC_LIST})