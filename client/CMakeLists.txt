cmake_minimum_required(VERSION 3.4)
project(SEALDemo VERSION 1.0)
add_executable(client app.cpp)

find_package(SEAL)
target_link_libraries(client SEAL::seal)
