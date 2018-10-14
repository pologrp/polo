cmake_minimum_required(VERSION 3.0.2)

project(api LANGUAGES CXX)

find_package(ZeroMQ CONFIG REQUIRED)

file(GLOB SOURCES "src/*.cpp")

add_library(api SHARED ${SOURCES})
target_include_directories(api PUBLIC include)
target_link_libraries(api PRIVATE polo)
target_link_libraries(api PRIVATE libzmq)
install(TARGETS api
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION include)

add_subdirectory(external/polo)
