cmake_minimum_required(VERSION 3.9.0)

project(polo-tests)

find_package(GTest REQUIRED)
if (NOT TARGET polo::polo)
  find_package(polo CONFIG REQUIRED)
endif()

enable_testing()

add_subdirectory(boosting)
add_subdirectory(encoder)
add_subdirectory(step)
add_subdirectory(utility)
