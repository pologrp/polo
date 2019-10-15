add_executable(anatomy anatomy.cpp)
target_link_libraries(anatomy PRIVATE polo::polo)
add_test(NAME polo.examples.getting.anatomy COMMAND anatomy)
set_tests_properties(polo.examples.getting.anatomy PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 0.030*"
)

add_executable(svmdata svmdata.cpp)
target_link_libraries(svmdata PRIVATE polo::polo)
add_test(NAME polo.examples.getting.svmdata COMMAND svmdata)
set_tests_properties(polo.examples.getting.svmdata PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 229.222"
)

add_executable(logger logger.cpp)
target_link_libraries(logger PRIVATE polo::polo)
add_test(NAME polo.examples.getting.logger COMMAND logger)
set_tests_properties(polo.examples.getting.logger PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 229.222"
)

add_executable(terminator terminator.cpp)
target_link_libraries(terminator PRIVATE polo::polo)
add_test(NAME polo.examples.getting.terminator COMMAND terminator)
set_tests_properties(polo.examples.getting.terminator PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 222.288758"
)

add_executable(loss loss.cpp)
target_link_libraries(loss PRIVATE polo::polo)
add_test(NAME polo.examples.getting.loss COMMAND loss)
set_tests_properties(polo.examples.getting.loss PROPERTIES
  PASS_REGULAR_EXPRESSION "Optimum: 0.000000"
)
