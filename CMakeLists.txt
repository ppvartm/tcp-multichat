cmake_minimum_required(VERSION 3.11)

project(tcp_multichat_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_executable(tcp_multichat_server
src/main.cpp
src/tcp_server.h
src/tcp_server.cpp
src/handler.h
)

target_link_libraries(tcp_multichat_server PRIVATE Threads::Threads CONAN_PKG::boost)
