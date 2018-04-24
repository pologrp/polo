cmake_minimum_required(VERSION 3.0.2)

project(
  POLO
  VERSION 1.0.0
  DESCRIPTION "POLO: POLicy-Based Optimization Framework."
  LANGUAGES CXX
)

add_library(polo INTERFACE)
target_include_directories(
  polo INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
install(TARGETS polo EXPORT polo)
install(EXPORT polo FILE polo-config.cmake
        DESTINATION share/cmake/polo)
install(DIRECTORY include/polo DESTINATION include)
install(FILES include/polo.hpp DESTINATION include)
