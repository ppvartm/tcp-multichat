cmake_minimum_required(VERSION 3.11)

project(tcp_multichat_client CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_executable(tcp_multichat_client
src/main.cpp
)

target_link_libraries(tcp_multichat_client PRIVATE Threads::Threads CONAN_PKG::boost)
