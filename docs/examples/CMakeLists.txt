cmake_minimum_required(VERSION 3.9.0)

project(polo-examples)

if (NOT TARGET polo::polo)
  find_package(polo CONFIG REQUIRED)
endif()

if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/data/australian_scale)
  file(
    DOWNLOAD
      https://www.csie.ntu.edu.tw/~cjlin/libsvmtools/datasets/binary/australian_scale
      ${CMAKE_CURRENT_BINARY_DIR}/data/australian_scale
    SHOW_PROGRESS
    EXPECTED_HASH
      SHA256=86799bcc4dbcd41929a514fa181d1c1a40e01f009dee139d4141a194d7ecd9e3
  )
endif()

enable_testing()

add_subdirectory(getting-started)
add_subdirectory(serial)
