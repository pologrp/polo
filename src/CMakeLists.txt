cmake_minimum_required(VERSION 3.9.0)

project(polo VERSION 1.0.0 LANGUAGES C CXX)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(Threads REQUIRED)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)
find_package(ZeroMQ REQUIRED)
find_package(cereal REQUIRED)
find_package(CURL QUIET)

add_library(polo INTERFACE)
add_library(polo::polo ALIAS polo)

target_compile_features(polo INTERFACE cxx_std_11)

target_include_directories(polo
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(polo
  INTERFACE
    Threads::Threads
    libzmq
    cereal
    ${BLAS_LIBRARIES}
    ${LAPACK_LIBRARIES}
)

if (CURL_FOUND)
  set(POLO_DEPENDS_CURL "find_dependency(CURL)")
  target_compile_definitions(polo INTERFACE POLO_WITH_CURL)
  target_include_directories(polo
    INTERFACE
      ${CURL_INCLUDE_DIRS}
  )
  target_link_libraries(polo
    INTERFACE
      ${CURL_LIBRARIES}
  )
endif()

install(TARGETS polo EXPORT polo)

install(EXPORT polo
  FILE polo-targets.cmake
  NAMESPACE polo::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polo
)

configure_package_config_file(polo-config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/polo-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/polo
)

write_basic_package_version_file(polo-config-version.cmake
  COMPATIBILITY SameMajorVersion
)

install(
  DIRECTORY
    include/polo/
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/polo
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/polo-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/polo-config-version.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/polo
)

include(CPack)
