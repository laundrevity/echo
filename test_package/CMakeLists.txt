cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

find_package(echo CONFIG REQUIRED)

add_executable(echo_test src/main.cpp)
target_link_libraries(echo_test 
    echo::echo
    pthread
)