cmake_minimum_required(VERSION 3.15)
project(echo CXX)



add_definitions("-std=c++17")

add_library(echo include/EchoServer.cpp)
target_include_directories(echo PUBLIC include)

set_target_properties(echo PROPERTIES PUBLIC_HEADER "include/EchoServer.h")
install(TARGETS echo)

# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()

# add_executable(echo_server main.cpp EchoServer.cpp)
# target_link_libraries(echo_server ${CONAN_LIBS})