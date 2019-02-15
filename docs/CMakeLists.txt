cmake_minimum_required(VERSION 3.9.0)

project(polo-docs)

if (NOT TARGET polo::polo)
  find_package(polo CONFIG REQUIRED)
endif()

add_subdirectory(examples)
