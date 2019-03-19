add_executable(anatomy anatomy.cpp)
target_link_libraries(anatomy polo::polo)
add_test(NAME polo.examples.getting.anatomy COMMAND anatomy)
set_tests_properties(polo.examples.getting.anatomy PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 0.030*"
)

add_executable(svmdata svmdata.cpp)
target_link_libraries(svmdata polo::polo)
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
add_test(NAME polo.examples.getting.svmdata COMMAND svmdata)
set_tests_properties(polo.examples.getting.svmdata PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 229.22*"
)

add_executable(logger logger.cpp)
target_link_libraries(logger polo::polo)
add_test(NAME polo.examples.getting.logger COMMAND logger)
set_tests_properties(polo.examples.getting.logger PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 229.22*"
)
