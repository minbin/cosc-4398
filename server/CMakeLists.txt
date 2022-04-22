cmake_minimum_required(VERSION 3.10)
project(SEALDemo VERSION 1.0)
add_executable(server app.cpp)

find_package(SEAL)
target_link_libraries(server SEAL::seal)
