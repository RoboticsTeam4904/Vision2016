cmake_minimum_required(VERSION 2.8)

project(highgoal.bin)

add_compile_options(-std=c++11)

find_package(OpenCV REQUIRED)

add_executable(highgoal.bin highgoal.cpp)
add_library(server ctsocket.cpp ctserver.cpp)
add_library(preprocess preprocess.cpp)
add_library(findgoal findgoal.cpp)

target_link_libraries(highgoal.bin server)
target_link_libraries(highgoal.bin preprocess)
target_link_libraries(highgoal.bin findgoal)

target_link_libraries(highgoal.bin ${OpenCV_LIBS})
target_link_libraries(highgoal.bin pthread)
