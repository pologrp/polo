cmake_minimum_required(VERSION 3.9.0)

project(polo-super)

add_subdirectory(src)
add_subdirectory(c-api)
add_subdirectory(docs/examples)

enable_testing()
add_subdirectory(tests)
