cmake_minimum_required(VERSION 3.14)
project(HighLoadWebServer)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system filesystem thread)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/)
include_directories(SYSTEM ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(HighLoadWebServer source/main.cpp source/ServerConfigParser.cpp source/Server.cpp source/request.cpp
        source/response.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES})
