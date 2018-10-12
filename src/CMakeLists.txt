cmake_minimum_required(VERSION 3.9.0)

project(polo VERSION 1.0.0 LANGUAGES C CXX)

find_package(Threads REQUIRED)
find_package(LAPACK REQUIRED NAMES OpenBLAS lapack)
find_package(ZeroMQ REQUIRED CONFIG)
find_package(cereal REQUIRED CONFIG)

add_library(polo INTERFACE)
add_library(polo::polo ALIAS polo)

target_include_directories(polo
  INTERFACE
    $<BUILD_INTERFACE:${polo_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(polo
  INTERFACE
    Threads::Threads
    libzmq
    cereal
)
if (TARGET OpenBLAS::OpenBLAS)
  target_link_libraries(polo
    INTERFACE
      OpenBLAS::OpenBLAS
  )
else()
  target_link_libraries(polo
    INTERFACE
      blas
      lapack
  )
endif()

install(TARGETS polo EXPORT polo
  INCLUDES DESTIONATION include
)

install(EXPORT polo
  FILE polo-targets.cmake
  NAMESPACE polo::
  DESTINATION lib/cmake/polo
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("polo-config-version.cmake"
  VERSION ${polo_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
  DIRECTORY
    include/polo/
  DESTINATION
    include/polo
)
install(
  FILES
    "polo-config.cmake"
    "${polo_BINARY_DIR}/polo-config-version.cmake"
  DESTINATION
    lib/cmake/polo
)
