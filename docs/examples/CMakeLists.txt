cmake_minimum_required(VERSION 3.9.0)

project(polo-examples)

if (NOT TARGET polo::polo)
  find_package(polo CONFIG REQUIRED)
endif()

enable_testing()

add_subdirectory(getting-started)
